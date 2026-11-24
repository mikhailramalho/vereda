#include "encode/encoder.h"

#include <llvm/ADT/APSInt.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/Support/raw_ostream.h>
#include <cmath>

using namespace llvm;
using camada::SMTSortRef;

namespace vereda::encode {

std::string describe(const Value *v) {
  std::string str;
  raw_string_ostream os(str);
  v->print(os);
  return str;
}

static std::string describe(const Type *ty) {
  std::string str;
  raw_string_ostream os(str);
  ty->print(os);
  return str;
}

// The format of a floating-point type that is an IEEE interchange format;
// null for x86_fp80 and ppc_fp128, which are not.
static const fltSemantics *ieee(Type *ty) {
  if (!ty->isFloatingPointTy() || ty->isX86_FP80Ty() || ty->isPPC_FP128Ty())
    return nullptr;
  return &ty->getFltSemantics();
}

// The element width of a vector of integers (not i1) or IEEE floats; 0 for
// anything else.
static unsigned vectorElementWidth(Type *ty) {
  auto *vt = dyn_cast<FixedVectorType>(ty);
  if (!vt)
    return 0;
  Type *e = vt->getElementType();
  if ((e->isIntegerTy() && !e->isIntegerTy(1)) || ieee(e))
    return e->getPrimitiveSizeInBits();
  return 0;
}

// The width of ty's bits as a field of a packed struct: an integer (i1 as
// one bit), a pointer, an IEEE float, a vector of these, or a struct of all
// of these, as clang returns small structs on x86-64 ({ptr, i32}); 0 for
// anything else.
unsigned Encoder::packedWidth(Type *ty) {
  if (ty->isIntegerTy())
    return ty->getIntegerBitWidth();
  if (ty->isPointerTy())
    return mem.pointerSort()->getWidth();
  if (ieee(ty))
    return ty->getPrimitiveSizeInBits();
  if (unsigned w = vectorElementWidth(ty))
    return w * cast<FixedVectorType>(ty)->getNumElements();
  auto *st = dyn_cast<StructType>(ty);
  if (!st)
    return 0;
  unsigned width = 0;
  for (Type *field : st->elements()) {
    unsigned w = packedWidth(field);
    if (!w)
      return 0;
    width += w;
  }
  return width;
}

// A field's value as the bits a packed struct holds, and back.
Term Encoder::toField(Term v, Type *ty) {
  return ty->isIntegerTy(1) ? fromBool(v, 1) : toBits(v, ty);
}

Term Encoder::fromField(Term bits, Type *ty) {
  return ty->isIntegerTy(1) ? toBool(bits) : fromBits(bits, ty);
}

// v with the bits from low up replaced by part.
Term Encoder::replaceBits(Term v, unsigned low, Term part) {
  unsigned high = low + part.width(), all = v.width();
  if (high < all)
    part = t.concat(t.extract(v, all - 1, high), part);
  return low > 0 ? t.concat(part, t.extract(v, low - 1, 0)) : part;
}

SMTSortRef Encoder::sort(Type *ty) {
  if (ty->isIntegerTy(1))
    return s.mkBoolSort();
  if (ieee(ty))
    return native() ? fpSort(ty) : s.mkBVSort(ty->getPrimitiveSizeInBits());
  if (unsigned w = vectorElementWidth(ty))
    return s.mkBVSort(w * cast<FixedVectorType>(ty)->getNumElements());
  if (ty->isPointerTy())
    return mem.pointerSort();
  if (unsigned width = packedWidth(ty))
    return s.mkBVSort(width);
  throw Unsupported("type " + describe(ty));
}

Term Encoder::fresh(Type *ty, const std::string &name) {
  std::string id = name + "#" + std::to_string(freshCount++);
  // A pointer from outside the program is an address converted to one, so
  // it points to no object the program kept to itself (memory.h, #137).
  if (ty->isPointerTy())
    return mem.fromAddress(t.symbol(id, s.mkBVSort(mem.pointerBits())));
  return t.symbol(id, sort(ty));
}

Term Encoder::value(const Value *v) {
  if (auto it = env.find(v); it != env.end()) {
    const memory::Cell *c = facts.find(key(it->second));
    return c ? c->data : it->second;
  }
  if (auto *ci = dyn_cast<ConstantInt>(v))
    return ci->getBitWidth() == 1 ? t.boolean(ci->isOne()) : t.bv(ci->getValue());
  if (isa<UndefValue>(v)) // also poison
    return fresh(v->getType(), "undef");
  if (isa<ConstantPointerNull>(v))
    return mem.null();
  if (auto *cf = dyn_cast<ConstantFP>(v); cf && ieee(cf->getType()))
    return floatValue(cf->getValueAPF(), cf->getType());
  Type *ty = v->getType();
  if ((vectorElementWidth(ty) || (ty->isStructTy() && packedWidth(ty))) && isa<Constant>(v)) {
    auto *c = cast<Constant>(v);
    unsigned n = ty->isStructTy() ? ty->getStructNumElements()
                                  : cast<FixedVectorType>(ty)->getNumElements();
    auto element = [&](unsigned i) {
      Constant *e = c->getAggregateElement(i); // null for some constant expressions
      if (!e)
        throw Unsupported("value " + describe(v));
      return toField(value(e), e->getType());
    };
    Term r = element(n - 1);
    for (unsigned i = n - 1; i-- > 0;)
      r = t.concat(r, element(i));
    return r;
  }
  if (auto *g = dyn_cast<GlobalVariable>(v))
    return mem.global(g);
  if (auto *ce = dyn_cast<ConstantExpr>(v)) {
    if (auto *g = dyn_cast<GEPOperator>(ce))
      return gep(*g);
    if (ce->isCast())
      return castTo(ce->getOpcode(), ce->getOperand(0), ce->getType());
    if (Instruction::isBinaryOp(ce->getOpcode()))
      return binary(ce->getOpcode(), ce->getOperand(0), ce->getOperand(1));
  }
  if (auto *f = dyn_cast<Function>(v))
    return mem.global(f);
  throw Unsupported("value " + describe(v));
}

Term Encoder::resize(Term v, unsigned width, bool isSigned) {
  if (v.isBool())
    v = fromBool(v, 1);
  unsigned from = v.width();
  if (from < width)
    return isSigned ? t.sext(v, width - from) : t.zext(v, width - from);
  return from > width ? t.extract(v, width - 1, 0) : v;
}

Term Encoder::gep(const GEPOperator &g) {
  unsigned bits = mem.pointerBits();
  APInt offset(bits, 0);
  SmallMapVector<Value *, APInt, 4> variable;
  if (g.getType()->isVectorTy() ||
      !g.collectOffset(mem.layout(), bits, variable, offset))
    throw Unsupported("getelementptr " + describe(&g));
  Term delta = t.bv(offset);
  for (auto &[index, scale] : variable)
    delta = t.add(delta, t.mul(resize(value(index), bits, true), t.bv(scale)));
  return mem.advance(value(g.getPointerOperand()), delta);
}

SMTSortRef Encoder::fpSort(Type *ty) {
  const fltSemantics *sem = ieee(ty);
  if (!sem)
    throw Unsupported("type " + describe(ty));
  unsigned precision = APFloat::semanticsPrecision(*sem); // with the hidden bit
  return s.mkFPSort(APFloat::semanticsSizeInBits(*sem) - precision, precision - 1,
                    fpEncoding);
}

camada::SMTExprRef Encoder::fp(const Value *v) {
  SMTSortRef sort = fpSort(v->getType()); // throws for x86_fp80, held as bits
  camada::SMTExprRef x = t.solverTerm(value(v));
  return native() ? x : s.mkBVToIEEEFP(x, sort);
}

Term Encoder::bits(const camada::SMTExprRef &f) {
  return t.fromSolver(native() ? f : s.mkIEEEFPToBV(f));
}

Term Encoder::toBits(Term v, Type *ty) {
  if (!native() || !ieee(ty))
    return v;
  // A constant's bits, except a NaN's, which are any NaN's as for every NaN term.
  if (auto it = floatBits.find(v.id());
      it != floatBits.end() && !APFloat(*ieee(ty), it->second).isNaN())
    return t.bv(it->second);
  return t.fromSolver(s.mkIEEEFPToBV(t.solverTerm(v)));
}

Term Encoder::fromBits(Term b, Type *ty) {
  if (!native() || !ieee(ty))
    return b;
  const APInt *c = t.constant(b);
  if (!c)
    return t.fromSolver(s.mkBVToIEEEFP(t.solverTerm(b), fpSort(ty)));
  Term &f = floatTerms[{ieee(ty), *c}];
  if (!f) {
    f = t.fromSolver(s.mkBVToIEEEFP(t.solverTerm(b), fpSort(ty)));
    floatBits.try_emplace(f.id(), *c);
  }
  return f;
}

std::optional<APFloat> Encoder::constFloat(const Value *v) {
  const fltSemantics *sem = ieee(v->getType());
  if (!sem || isa<UndefValue>(v)) // undef is a fresh symbol per use
    return std::nullopt;
  Term e = value(v);
  const APInt *c = nullptr;
  if (!native())
    c = t.constant(e);
  else if (auto it = floatBits.find(e.id()); it != floatBits.end())
    c = &it->second;
  if (!c)
    return std::nullopt;
  return APFloat(*sem, *c);
}

Term Encoder::floatValue(const APFloat &f, Type *ty) {
  return fromBits(t.bv(f.bitcastToAPInt()), ty);
}

Term Encoder::folded(const APFloat &r, Type *ty) {
  return r.isNaN() ? Term() : floatValue(r, ty);
}

Term Encoder::signBit(const Value *v) {
  Term b = toBits(value(v), v->getType());
  unsigned w = b.width();
  return t.eq(t.extract(b, w - 1, w - 1), t.bv(1, 1));
}

camada::SMTExprRef Encoder::rm(camada::RM mode) { return s.mkRM(mode, fpEncoding); }

Term Encoder::fpClass(const Term &x, Type *ty, unsigned mask) {
  if (!ieee(ty))
    throw Unsupported("type " + describe(ty));
  if (native()) {
    // The theory has one NaN, so either NaN class means any NaN; the sign
    // of a zero is told by structural equality, which -0 and +0 fail.
    const fltSemantics &sem = *ieee(ty);
    Term zero = fromBits(t.bv(APFloat::getZero(sem).bitcastToAPInt()), ty);
    Term negZero = fromBits(t.bv(APFloat::getZero(sem, true).bitcastToAPInt()), ty);
    camada::SMTExprRef f = t.solverTerm(x), z = t.solverTerm(zero);
    Term neg = t.fromSolver(s.mkFPLt(f, z)), pos = t.fromSolver(s.mkFPGt(f, z));
    Term inf = t.fromSolver(s.mkFPIsInfinite(f)), normal = t.fromSolver(s.mkFPIsNormal(f));
    Term sub = t.fromSolver(s.mkFPIsSubnormal(f));
    const std::pair<unsigned, Term> classes[] = {
        {fcNan, t.fromSolver(s.mkFPIsNaN(f))}, {fcNegInf, t.land(neg, inf)},
        {fcNegNormal, t.land(neg, normal)}, {fcNegSubnormal, t.land(neg, sub)},
        {fcNegZero, t.eq(x, negZero)},      {fcPosZero, t.eq(x, zero)},
        {fcPosSubnormal, t.land(pos, sub)}, {fcPosNormal, t.land(pos, normal)},
        {fcPosInf, t.land(pos, inf)}};
    Term r = t.boolean(false);
    for (auto &[bit, in] : classes)
      if (mask & bit)
        r = t.lor(r, in);
    return r;
  }
  const fltSemantics &sem = *ieee(ty);
  unsigned w = x.width(), sig = APFloat::semanticsPrecision(sem) - 1;
  Term exp = t.extract(x, w - 2, sig), mant = t.extract(x, sig - 1, 0);
  Term neg = t.eq(t.extract(x, w - 1, w - 1), t.bv(1, 1));
  Term top = t.eq(exp, t.bvnot(t.bv(0, w - 1 - sig)));
  Term low = t.eq(exp, t.bv(0, w - 1 - sig));
  Term noMant = t.eq(mant, t.bv(0, sig));
  Term nan = t.land(top, t.lnot(noMant));
  Term quiet = t.eq(t.extract(x, sig - 1, sig - 1), t.bv(1, 1));
  Term inf = t.land(top, noMant), zero = t.land(low, noMant);
  Term sub = t.land(low, t.lnot(noMant));
  Term normal = t.lnot(t.lor(top, low));
  Term pos = t.lnot(neg);
  const std::pair<unsigned, Term> classes[] = {
      {fcSNan, t.land(nan, t.lnot(quiet))}, {fcQNan, t.land(nan, quiet)},
      {fcNegInf, t.land(neg, inf)},         {fcNegNormal, t.land(neg, normal)},
      {fcNegSubnormal, t.land(neg, sub)},   {fcNegZero, t.land(neg, zero)},
      {fcPosZero, t.land(pos, zero)},       {fcPosSubnormal, t.land(pos, sub)},
      {fcPosNormal, t.land(pos, normal)},   {fcPosInf, t.land(pos, inf)}};
  Term r = t.boolean(false);
  for (auto &[bit, in] : classes)
    if (mask & bit)
      r = t.lor(r, in);
  return r;
}

Term Encoder::fcmp(CmpInst::Predicate p, const Value *lhs, const Value *rhs) {
  if (p == CmpInst::FCMP_FALSE || p == CmpInst::FCMP_TRUE)
    return t.boolean(p == CmpInst::FCMP_TRUE);
  if (auto a = constFloat(lhs), b = constFloat(rhs); a && b)
    return t.boolean(FCmpInst::compare(*a, *b, p));
  camada::SMTExprRef a = fp(lhs), b = fp(rhs);
  Term unordered = t.lor(t.fromSolver(s.mkFPIsNaN(a)), t.fromSolver(s.mkFPIsNaN(b)));
  if (p == CmpInst::FCMP_UNO || p == CmpInst::FCMP_ORD)
    return p == CmpInst::FCMP_UNO ? unordered : t.lnot(unordered);
  Term r;
  switch (CmpInst::getOrderedPredicate(p)) {
  case CmpInst::FCMP_OEQ: r = t.fromSolver(s.mkFPEqual(a, b)); break;
  case CmpInst::FCMP_ONE: r = t.lnot(t.fromSolver(s.mkFPEqual(a, b))); break;
  case CmpInst::FCMP_OGT: r = t.fromSolver(s.mkFPGt(a, b)); break;
  case CmpInst::FCMP_OGE: r = t.fromSolver(s.mkFPGe(a, b)); break;
  case CmpInst::FCMP_OLT: r = t.fromSolver(s.mkFPLt(a, b)); break;
  case CmpInst::FCMP_OLE: r = t.fromSolver(s.mkFPLe(a, b)); break;
  default: throw Unsupported("predicate " + std::string(CmpInst::getPredicateName(p)));
  }
  // Ordered: false on a NaN; unordered: true on one.
  return CmpInst::isOrdered(p) ? t.land(t.lnot(unordered), r) : t.lor(unordered, r);
}

Term Encoder::castTo(unsigned opcode, const Value *op, Type *dst) {
  Type *src = op->getType();
  Term a = value(op);
  switch (opcode) {
  case Instruction::FPToSI:
  case Instruction::FPToUI: { // toward zero, as C converts
    if (!dst->isIntegerTy())
      throw Unsupported("cast " + describe(dst));
    unsigned to = dst->getIntegerBitWidth();
    // In range only: out of it (or NaN, infinite) the encoding gives the
    // solver's value, not APFloat's.
    if (auto c = constFloat(op)) {
      APSInt r(to, opcode == Instruction::FPToUI);
      bool exact;
      if (!(c->convertToInteger(r, APFloat::rmTowardZero, &exact) & APFloat::opInvalidOp))
        return to == 1 ? t.boolean(r[0]) : t.bv(r);
    }
    Term r = t.fromSolver(opcode == Instruction::FPToSI ? s.mkFPToSBV(fp(op), to)
                                                        : s.mkFPToUBV(fp(op), to));
    return to == 1 ? t.eq(r, t.bv(1, 1)) : r;
  }
  case Instruction::SIToFP:
  case Instruction::UIToFP: {
    if (src->isIntegerTy(1))
      a = opcode == Instruction::SIToFP ? t.ite(a, t.bv(-1, 1), t.bv(0, 1)) : fromBool(a, 1);
    if (const APInt *c = t.constant(a); c && ieee(dst)) {
      APFloat r(*ieee(dst));
      r.convertFromAPInt(*c, opcode == Instruction::SIToFP, APFloat::rmNearestTiesToEven);
      return floatValue(r, dst);
    }
    camada::SMTExprRef x = t.solverTerm(a);
    return bits(opcode == Instruction::SIToFP ? s.mkSBVToFP(x, fpSort(dst), rm(camada::RM::ROUND_TO_EVEN))
                                              : s.mkUBVToFP(x, fpSort(dst), rm(camada::RM::ROUND_TO_EVEN)));
  }
  case Instruction::FPExt:
  case Instruction::FPTrunc:
    if (auto c = constFloat(op); c && ieee(dst)) {
      bool loses;
      c->convert(*ieee(dst), APFloat::rmNearestTiesToEven, &loses);
      if (Term r = folded(*c, dst))
        return r;
    }
    return bits(s.mkFPToFP(fp(op), fpSort(dst), rm(camada::RM::ROUND_TO_EVEN)));
  default:
    break;
  }
  // A bitcast between a float and an integer, or another float, of the same
  // width keeps the bits.
  if (opcode == Instruction::BitCast && !src->isPointerTy() &&
      (ieee(src) || ieee(dst)) && src->getPrimitiveSizeInBits() == dst->getPrimitiveSizeInBits() &&
      (ieee(src) || src->isIntegerTy()) && (ieee(dst) || dst->isIntegerTy()))
    return fromBits(toBits(a, src), dst);
  switch (opcode) {
  case Instruction::PtrToInt:
    return dst->isIntegerTy(1) ? toBool(resize(mem.address(a), 1, false))
                               : resize(mem.address(a), dst->getIntegerBitWidth(), false);
  case Instruction::IntToPtr:
    return mem.fromAddress(resize(a, mem.pointerBits(), false));
  case Instruction::BitCast:
    if (src->isPointerTy() && dst->isPointerTy())
      return a;
    break;
  default:
    break;
  }
  if (!src->isIntegerTy() || !dst->isIntegerTy())
    throw Unsupported("cast " + std::string(Instruction::getOpcodeName(opcode)) + " from " +
                      describe(op));
  unsigned from = src->getIntegerBitWidth(), to = dst->getIntegerBitWidth();
  switch (opcode) {
  case Instruction::ZExt:
    return from == 1 ? fromBool(a, to) : t.zext(a, to - from);
  case Instruction::SExt:
    return from == 1 ? t.ite(a, t.bvnot(t.bv(0, to)), t.bv(0, to))
                     : t.sext(a, to - from);
  case Instruction::Trunc: {
    Term r = t.extract(a, to - 1, 0);
    return to == 1 ? t.eq(r, t.bv(1, 1)) : r;
  }
  default:
    throw Unsupported("cast " + std::string(Instruction::getOpcodeName(opcode)));
  }
}

Term Encoder::toBool(Term bv) {
  return t.lnot(t.eq(bv, t.bv(0, bv.width())));
}

Term Encoder::fromBool(Term b, unsigned width) {
  return t.ite(b, t.bv(1, width), t.bv(0, width));
}

Term Encoder::isTrue(const Value *v) {
  if (ieee(v->getType())) { // nonzero, as C reads a scalar: NaN is true
    if (auto c = constFloat(v))
      return t.boolean(!c->isZero());
    Term zero = fromBits(t.bv(0, v->getType()->getPrimitiveSizeInBits()), v->getType());
    camada::SMTExprRef f = fp(v), z = t.solverTerm(zero);
    if (!native())
      z = s.mkBVToIEEEFP(z, fpSort(v->getType()));
    return t.lnot(t.fromSolver(s.mkFPEqual(f, z)));
  }
  Term t = value(v);
  return v->getType()->isIntegerTy(1) ? t : toBool(t);
}

Term Encoder::binary(unsigned opcode, const Value *lhs, const Value *rhs) {
  if (lhs->getType()->isFloatingPointTy()) {
    if (auto x = constFloat(lhs), y = constFloat(rhs); x && y) {
      auto even = APFloat::rmNearestTiesToEven;
      switch (opcode) {
      case Instruction::FAdd: x->add(*y, even); break;
      case Instruction::FSub: x->subtract(*y, even); break;
      case Instruction::FMul: x->multiply(*y, even); break;
      case Instruction::FDiv: x->divide(*y, even); break;
      case Instruction::FRem: x->mod(*y); break; // exact, truncating as fmod
      }
      if (Term r = folded(*x, lhs->getType()))
        return r;
    }
    camada::SMTExprRef a = fp(lhs), b = fp(rhs), even = rm(camada::RM::ROUND_TO_EVEN);
    switch (opcode) {
    case Instruction::FAdd: return bits(s.mkFPAdd(a, b, even));
    case Instruction::FSub: return bits(s.mkFPSub(a, b, even));
    case Instruction::FMul: return bits(s.mkFPMul(a, b, even));
    case Instruction::FDiv: return bits(s.mkFPDiv(a, b, even));
    default: // frem is C's fmod, which truncates; SMT's fp.rem rounds to nearest
      throw Unsupported(std::string("instruction ") + Instruction::getOpcodeName(opcode));
    }
  }
  bool isBool = lhs->getType()->isIntegerTy(1);
  Term a = value(lhs), b = value(rhs);
  switch (opcode) {
  case Instruction::And: return isBool ? t.land(a, b) : t.bvand(a, b);
  case Instruction::Or:  return isBool ? t.lor(a, b) : t.bvor(a, b);
  case Instruction::Xor: return isBool ? t.lxor(a, b) : t.bvxor(a, b);
  default: break;
  }
  if (isBool) { // arithmetic on i1: compute in BV1
    a = fromBool(a, 1);
    b = fromBool(b, 1);
  }
  Term r;
  switch (opcode) {
  case Instruction::Add:  r = t.add(a, b); break;
  case Instruction::Sub:  r = t.sub(a, b); break;
  case Instruction::Mul:  r = t.mul(a, b); break;
  case Instruction::UDiv: r = t.udiv(a, b); break;
  case Instruction::SDiv: r = t.sdiv(a, b); break;
  case Instruction::URem: r = t.urem(a, b); break;
  case Instruction::SRem: r = t.srem(a, b); break;
  case Instruction::Shl:  r = t.shl(a, b); break;
  case Instruction::LShr: r = t.lshr(a, b); break;
  case Instruction::AShr: r = t.ashr(a, b); break;
  default: throw Unsupported(std::string("instruction ") + Instruction::getOpcodeName(opcode));
  }
  return isBool ? toBool(r) : r;
}

void Encoder::encode(Instruction &inst) {
  Term r;
  if (isa<BinaryOperator>(inst)) {
    r = binary(inst.getOpcode(), inst.getOperand(0), inst.getOperand(1));
  } else if (inst.getOpcode() == Instruction::FNeg) { // flips the sign bit, NaN or not
    Term a = value(inst.getOperand(0));
    if (auto c = constFloat(inst.getOperand(0)); c && !c->isNaN())
      r = floatValue(-*c, inst.getType());
    else
      r = native() && ieee(inst.getType()) ? t.fromSolver(s.mkFPNeg(t.solverTerm(a)))
                                           : t.bvxor(a, t.bv(APInt::getSignMask(a.width())));
  } else if (auto *fc = dyn_cast<FCmpInst>(&inst)) {
    r = fcmp(fc->getPredicate(), fc->getOperand(0), fc->getOperand(1));
  } else if (auto *cmp = dyn_cast<ICmpInst>(&inst)) {
    Term a = value(cmp->getOperand(0)), b = value(cmp->getOperand(1));
    // Pointers are equal as Memory::equal says; ordered comparison
    // compares addresses.
    if (cmp->getOperand(0)->getType()->isPointerTy()) {
      if (cmp->isEquality()) {
        r = mem.equal(a, b);
        if (cmp->getPredicate() == CmpInst::ICMP_NE)
          r = t.lnot(r);
        bind(&inst, r);
        return;
      }
      a = mem.address(a);
      b = mem.address(b);
    }
    bool isBool = cmp->getOperand(0)->getType()->isIntegerTy(1);
    if (isBool && !cmp->isEquality()) {
      a = fromBool(a, 1);
      b = fromBool(b, 1);
    }
    switch (cmp->getPredicate()) {
    case CmpInst::ICMP_EQ:  r = t.eq(a, b); break;
    case CmpInst::ICMP_NE:  r = t.lnot(t.eq(a, b)); break;
    case CmpInst::ICMP_UGT: r = t.ugt(a, b); break;
    case CmpInst::ICMP_UGE: r = t.uge(a, b); break;
    case CmpInst::ICMP_ULT: r = t.ult(a, b); break;
    case CmpInst::ICMP_ULE: r = t.ule(a, b); break;
    case CmpInst::ICMP_SGT: r = t.sgt(a, b); break;
    case CmpInst::ICMP_SGE: r = t.sge(a, b); break;
    case CmpInst::ICMP_SLT: r = t.slt(a, b); break;
    case CmpInst::ICMP_SLE: r = t.sle(a, b); break;
    default: throw Unsupported("predicate " + std::string(cmp->getPredicateName(cmp->getPredicate())));
    }
  } else if (auto *sel = dyn_cast<SelectInst>(&inst)) {
    r = t.ite(value(sel->getCondition()), value(sel->getTrueValue()),
                value(sel->getFalseValue()));
  } else if (auto *c = dyn_cast<CastInst>(&inst)) {
    r = castTo(c->getOpcode(), c->getOperand(0), c->getDestTy());
  } else if (auto *g = dyn_cast<GetElementPtrInst>(&inst)) {
    r = gep(*llvm::cast<GEPOperator>(g));
  } else if (isa<FreezeInst>(inst)) {
    r = value(inst.getOperand(0));
  } else if (isa<ExtractElementInst>(inst) || isa<InsertElementInst>(inst)) {
    // Constant indexes only: the scalarizer turns variable ones into selects.
    Value *vec = inst.getOperand(0);
    unsigned w = vectorElementWidth(vec->getType());
    auto *index = dyn_cast<ConstantInt>(inst.getOperand(isa<ExtractElementInst>(inst) ? 1 : 2));
    unsigned n = w ? cast<FixedVectorType>(vec->getType())->getNumElements() : 0;
    if (!w || !index || index->getZExtValue() >= n)
      throw Unsupported("instruction " + describe(&inst));
    unsigned low = w * index->getZExtValue();
    Term v = value(vec);
    Type *elem = cast<FixedVectorType>(vec->getType())->getElementType();
    if (isa<ExtractElementInst>(inst)) {
      r = fromBits(t.extract(v, low + w - 1, low), elem);
    } else {
      r = replaceBits(v, low, toBits(value(inst.getOperand(1)), elem));
    }
  } else if (isa<ExtractValueInst>(inst) || isa<InsertValueInst>(inst)) {
    // The field's bits within the packed struct, at its indices.
    Value *agg = inst.getOperand(0);
    if (!agg->getType()->isStructTy() || !packedWidth(agg->getType()))
      throw Unsupported("instruction " + describe(&inst));
    auto *ev = dyn_cast<ExtractValueInst>(&inst);
    ArrayRef<unsigned> indices =
        ev ? ev->getIndices() : cast<InsertValueInst>(inst).getIndices();
    Type *field = agg->getType();
    unsigned low = 0;
    for (unsigned i : indices) { // a packed type holds no arrays
      auto *st = cast<StructType>(field);
      for (unsigned j = 0; j < i; ++j)
        low += packedWidth(st->getElementType(j));
      field = st->getElementType(i);
    }
    if (ev)
      r = fromField(t.extract(value(agg), low + packedWidth(field) - 1, low), field);
    else
      r = replaceBits(value(agg), low, toField(value(inst.getOperand(1)), field));
  } else {
    throw Unsupported(std::string("instruction ") + inst.getOpcodeName());
  }
  bind(&inst, r);
}

bool Encoder::fpIntrinsic(CallBase &c) {
  Intrinsic::ID id = c.getCalledFunction()->getIntrinsicID();
  auto arg = [&](unsigned i) { return c.getArgOperand(i); };
  auto integral = [&](camada::RM mode) {
    return bits(s.mkFPToIntegral(fp(arg(0)), rm(mode)));
  };
  Term r;
  switch (id) {
  case Intrinsic::fabs:
  case Intrinsic::copysign: { // operations on the sign, NaN or not
    Term a = value(arg(0));
    if (native() && ieee(arg(0)->getType())) { // x86_fp80 stays bits
      if (auto c = constFloat(arg(0)); c && !c->isNaN()) {
        r = floatValue(abs(*c), arg(0)->getType());
        if (id == Intrinsic::copysign) // a constant sign folds the ite
          r = t.ite(signBit(arg(1)), floatValue(-abs(*c), arg(0)->getType()), r);
        break;
      }
      r = t.fromSolver(s.mkFPAbs(t.solverTerm(a)));
      if (id == Intrinsic::copysign)
        r = t.ite(signBit(arg(1)), t.fromSolver(s.mkFPNeg(t.solverTerm(r))), r);
      break;
    }
    Term sign = t.bv(APInt::getSignMask(a.width()));
    r = t.bvand(a, t.bvnot(sign));
    if (id == Intrinsic::copysign)
      r = t.bvor(r, t.bvand(value(arg(1)), sign));
    break;
  }
  case Intrinsic::sqrt: {
    // APFloat has no square root; the host's is correctly rounded, for
    // float and double.
    Type *ty = arg(0)->getType();
    auto c = constFloat(arg(0));
    if (c && ty->isFloatTy())
      r = folded(APFloat(std::sqrt(c->convertToFloat())), ty);
    else if (c && ty->isDoubleTy())
      r = folded(APFloat(std::sqrt(c->convertToDouble())), ty);
    if (!r)
      r = bits(s.mkFPSqrt(fp(arg(0)), rm(camada::RM::ROUND_TO_EVEN)));
    break;
  }
  case Intrinsic::fma:
  case Intrinsic::fmuladd: // fused: one rounding, which fmuladd allows
    if (auto x = constFloat(arg(0)), y = constFloat(arg(1)), z = constFloat(arg(2)); x && y && z) {
      x->fusedMultiplyAdd(*y, *z, APFloat::rmNearestTiesToEven);
      if ((r = folded(*x, arg(0)->getType())))
        break;
    }
    r = bits(s.mkFPFMA(fp(arg(0)), fp(arg(1)), fp(arg(2)), rm(camada::RM::ROUND_TO_EVEN)));
    break;
  case Intrinsic::floor: r = integral(camada::RM::ROUND_TO_MINUS_INF); break;
  case Intrinsic::ceil: r = integral(camada::RM::ROUND_TO_PLUS_INF); break;
  case Intrinsic::trunc: r = integral(camada::RM::ROUND_TO_ZERO); break;
  case Intrinsic::round: r = integral(camada::RM::ROUND_TO_AWAY); break;
  case Intrinsic::rint:
  case Intrinsic::nearbyint:
  case Intrinsic::roundeven: r = integral(camada::RM::ROUND_TO_EVEN); break;
  case Intrinsic::minnum:
  case Intrinsic::maxnum: { // a NaN operand gives the other; a signed zero either
    if (auto x = constFloat(arg(0)), y = constFloat(arg(1)); x && y) {
      APFloat::cmpResult cmp = x->compare(*y);
      bool first = cmp == APFloat::cmpEqual ||
                   cmp == (id == Intrinsic::minnum ? APFloat::cmpLessThan : APFloat::cmpGreaterThan);
      r = value(arg(!x->isNaN() && (y->isNaN() || first) ? 0 : 1));
      break;
    }
    camada::SMTExprRef a = fp(arg(0)), b = fp(arg(1));
    Term first = t.fromSolver(id == Intrinsic::minnum ? s.mkFPLe(a, b) : s.mkFPGe(a, b));
    r = t.ite(t.fromSolver(s.mkFPIsNaN(a)), value(arg(1)),
              t.ite(t.lor(t.fromSolver(s.mkFPIsNaN(b)), first), value(arg(0)), value(arg(1))));
    break;
  }
  case Intrinsic::is_fpclass:
    // Natively either NaN class is any NaN, as fpClass has it; with fp2bv
    // fpClass reads the constant's bits and folds.
    if (auto c = constFloat(arg(0)); c && native()) {
      unsigned mask = cast<ConstantInt>(arg(1))->getZExtValue();
      r = t.boolean(mask & (c->isNaN() ? fcNan : c->classify()));
      break;
    }
    r = fpClass(value(arg(0)), arg(0)->getType(),
                cast<ConstantInt>(arg(1))->getZExtValue());
    break;
  default:
    return false;
  }
  bind(&c, r);
  return true;
}

bool Encoder::intrinsic(CallBase &c) {
  Function *f = c.getCalledFunction();
  if (fpIntrinsic(c))
    return true;
  // What InstCombine could not fold, the back end lowers to "unknown", as
  // LowerConstantIntrinsics does: objectsize -1 (0 for its minimum), and
  // is.constant false.
  if (f->getIntrinsicID() == Intrinsic::objectsize) {
    bool min = cast<ConstantInt>(c.getArgOperand(1))->isOne();
    unsigned w = c.getType()->getIntegerBitWidth();
    bind(&c, min ? t.bv(0, w) : t.bvnot(t.bv(0, w)));
    return true;
  }
  if (f->getIntrinsicID() == Intrinsic::is_constant) {
    bind(&c, t.boolean(false));
    return true;
  }
  if (f->getIntrinsicID() == Intrinsic::bswap) { // the low byte goes highest
    Term a = value(c.getArgOperand(0)), r = t.extract(a, 7, 0);
    for (unsigned lo = 8; lo < a.width(); lo += 8)
      r = t.concat(r, t.extract(a, lo + 7, lo));
    bind(&c, r);
    return true;
  }
  // Funnel shifts, as LangRef defines them: shift the concatenation a:b by
  // the amount modulo N and keep the high (fshl) or low (fshr) N bits. The
  // 2N-bit shift never reaches 2N, so an amount of 0 gives a or b unshifted.
  if (f->getIntrinsicID() == Intrinsic::fshl || f->getIntrinsicID() == Intrinsic::fshr) {
    Term a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
    unsigned n = a.width();
    Term by = t.zext(t.urem(value(c.getArgOperand(2)), t.bv(n, n)), n);
    bind(&c, f->getIntrinsicID() == Intrinsic::fshl
                 ? t.extract(t.shl(t.concat(a, b), by), 2 * n - 1, n)
                 : t.extract(t.lshr(t.concat(a, b), by), n - 1, 0));
    return true;
  }
  Term (Terms::*op)(const Term &, const Term &) = nullptr;
  Term (Terms::*less)(const Term &, const Term &) = &Terms::slt;
  switch (f->getIntrinsicID()) { // InstCombine's min and max
  case Intrinsic::umin:
  case Intrinsic::umax: less = &Terms::ult; [[fallthrough]];
  case Intrinsic::smin:
  case Intrinsic::smax: {
    Term a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
    bool min = f->getIntrinsicID() == Intrinsic::umin || f->getIntrinsicID() == Intrinsic::smin;
    bind(&c, t.ite((t.*less)(a, b), min ? a : b, min ? b : a));
    return true;
  }
  default: break;
  }
  Term (Terms::*overflow)(const Term &, const Term &) = nullptr;
  switch (f->getIntrinsicID()) {
  case Intrinsic::sadd_with_overflow: op = &Terms::add; overflow = &Terms::saddo; break;
  case Intrinsic::uadd_with_overflow: op = &Terms::add; overflow = &Terms::uaddo; break;
  case Intrinsic::ssub_with_overflow: op = &Terms::sub; overflow = &Terms::ssubo; break;
  case Intrinsic::usub_with_overflow: op = &Terms::sub; overflow = &Terms::usubo; break;
  case Intrinsic::smul_with_overflow: op = &Terms::mul; overflow = &Terms::smulo; break;
  case Intrinsic::umul_with_overflow: op = &Terms::mul; overflow = &Terms::umulo; break;
  default: return false;
  }
  Term a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
  // {result, overflow bit}: the overflow bit packs above the result.
  bind(&c, t.concat(fromBool((t.*overflow)(a, b), 1), (t.*op)(a, b)));
  return true;
}

} // namespace vereda::encode
