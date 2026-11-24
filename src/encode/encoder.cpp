#include "encode/encoder.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using camada::SMTExprRef;
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
SMTExprRef Encoder::toField(SMTExprRef v, Type *ty) {
  return ty->isIntegerTy(1) ? fromBool(v, 1) : toBits(v, ty);
}

SMTExprRef Encoder::fromField(SMTExprRef bits, Type *ty) {
  return ty->isIntegerTy(1) ? toBool(bits) : fromBits(bits, ty);
}

// v with the bits from low up replaced by part.
SMTExprRef Encoder::replaceBits(SMTExprRef v, unsigned low, SMTExprRef part) {
  unsigned high = low + part->getWidth(), all = v->getWidth();
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

SMTExprRef Encoder::fresh(Type *ty, const std::string &name) {
  std::string id = name + "#" + std::to_string(freshCount++);
  // A pointer from outside the program is an address converted to one, so
  // it points to no object the program kept to itself (memory.h, #137).
  if (ty->isPointerTy())
    return mem.fromAddress(t.symbol(id, s.mkBVSort(mem.pointerBits())));
  return t.symbol(id, sort(ty));
}

SMTExprRef Encoder::value(const Value *v) {
  if (auto it = env.find(v); it != env.end())
    return it->second;
  if (auto *ci = dyn_cast<ConstantInt>(v))
    return ci->getBitWidth() == 1 ? t.boolean(ci->isOne()) : t.bv(ci->getValue());
  if (isa<UndefValue>(v)) // also poison
    return fresh(v->getType(), "undef");
  if (isa<ConstantPointerNull>(v))
    return mem.null();
  if (auto *cf = dyn_cast<ConstantFP>(v); cf && ieee(cf->getType()))
    return fromBits(t.bv(cf->getValueAPF().bitcastToAPInt()), cf->getType());
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
    SMTExprRef r = element(n - 1);
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

SMTExprRef Encoder::resize(SMTExprRef v, unsigned width, bool isSigned) {
  if (v->isBoolSort())
    v = fromBool(v, 1);
  unsigned from = v->getWidth();
  if (from < width)
    return isSigned ? t.sext(v, width - from) : t.zext(v, width - from);
  return from > width ? t.extract(v, width - 1, 0) : v;
}

SMTExprRef Encoder::gep(const GEPOperator &g) {
  unsigned bits = mem.pointerBits();
  APInt offset(bits, 0);
  SmallMapVector<Value *, APInt, 4> variable;
  if (g.getType()->isVectorTy() ||
      !g.collectOffset(mem.layout(), bits, variable, offset))
    throw Unsupported("getelementptr " + describe(&g));
  SMTExprRef delta = t.bv(offset);
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

SMTExprRef Encoder::fp(const Value *v) {
  SMTSortRef sort = fpSort(v->getType()); // throws for x86_fp80, held as bits
  return native() ? value(v) : s.mkBVToIEEEFP(value(v), sort);
}

SMTExprRef Encoder::bits(const SMTExprRef &f) { return native() ? f : s.mkIEEEFPToBV(f); }

SMTExprRef Encoder::toBits(SMTExprRef v, Type *ty) {
  return native() && ieee(ty) ? s.mkIEEEFPToBV(v) : v;
}

SMTExprRef Encoder::fromBits(SMTExprRef b, Type *ty) {
  return native() && ieee(ty) ? s.mkBVToIEEEFP(b, fpSort(ty)) : b;
}

SMTExprRef Encoder::signBit(const Value *v) {
  SMTExprRef b = toBits(value(v), v->getType());
  unsigned w = b->getWidth();
  return t.eq(t.extract(b, w - 1, w - 1), t.bv(1, 1));
}

SMTExprRef Encoder::rm(camada::RM mode) { return s.mkRM(mode, fpEncoding); }

SMTExprRef Encoder::fpClass(const SMTExprRef &x, Type *ty, unsigned mask) {
  if (!ieee(ty))
    throw Unsupported("type " + describe(ty));
  if (native()) {
    // The theory has one NaN, so either NaN class means any NaN; the sign
    // of a zero is told by structural equality, which -0 and +0 fail.
    const fltSemantics &sem = *ieee(ty);
    SMTExprRef zero = fromBits(t.bv(APFloat::getZero(sem).bitcastToAPInt()), ty);
    SMTExprRef negZero = fromBits(t.bv(APFloat::getZero(sem, true).bitcastToAPInt()), ty);
    SMTExprRef neg = s.mkFPLt(x, zero), pos = s.mkFPGt(x, zero);
    SMTExprRef inf = s.mkFPIsInfinite(x), normal = s.mkFPIsNormal(x);
    SMTExprRef sub = s.mkFPIsSubnormal(x);
    const std::pair<unsigned, SMTExprRef> classes[] = {
        {fcNan, s.mkFPIsNaN(x)},           {fcNegInf, t.land(neg, inf)},
        {fcNegNormal, t.land(neg, normal)}, {fcNegSubnormal, t.land(neg, sub)},
        {fcNegZero, t.eq(x, negZero)},      {fcPosZero, t.eq(x, zero)},
        {fcPosSubnormal, t.land(pos, sub)}, {fcPosNormal, t.land(pos, normal)},
        {fcPosInf, t.land(pos, inf)}};
    SMTExprRef r = t.boolean(false);
    for (auto &[bit, in] : classes)
      if (mask & bit)
        r = t.lor(r, in);
    return r;
  }
  const fltSemantics &sem = *ieee(ty);
  unsigned w = x->getWidth(), sig = APFloat::semanticsPrecision(sem) - 1;
  SMTExprRef exp = t.extract(x, w - 2, sig), mant = t.extract(x, sig - 1, 0);
  SMTExprRef neg = t.eq(t.extract(x, w - 1, w - 1), t.bv(1, 1));
  SMTExprRef top = t.eq(exp, t.bvnot(t.bv(0, w - 1 - sig)));
  SMTExprRef low = t.eq(exp, t.bv(0, w - 1 - sig));
  SMTExprRef noMant = t.eq(mant, t.bv(0, sig));
  SMTExprRef nan = t.land(top, t.lnot(noMant));
  SMTExprRef quiet = t.eq(t.extract(x, sig - 1, sig - 1), t.bv(1, 1));
  SMTExprRef inf = t.land(top, noMant), zero = t.land(low, noMant);
  SMTExprRef sub = t.land(low, t.lnot(noMant));
  SMTExprRef normal = t.lnot(t.lor(top, low));
  SMTExprRef pos = t.lnot(neg);
  const std::pair<unsigned, SMTExprRef> classes[] = {
      {fcSNan, t.land(nan, t.lnot(quiet))}, {fcQNan, t.land(nan, quiet)},
      {fcNegInf, t.land(neg, inf)},         {fcNegNormal, t.land(neg, normal)},
      {fcNegSubnormal, t.land(neg, sub)},   {fcNegZero, t.land(neg, zero)},
      {fcPosZero, t.land(pos, zero)},       {fcPosSubnormal, t.land(pos, sub)},
      {fcPosNormal, t.land(pos, normal)},   {fcPosInf, t.land(pos, inf)}};
  SMTExprRef r = t.boolean(false);
  for (auto &[bit, in] : classes)
    if (mask & bit)
      r = t.lor(r, in);
  return r;
}

SMTExprRef Encoder::fcmp(CmpInst::Predicate p, const Value *lhs, const Value *rhs) {
  if (p == CmpInst::FCMP_FALSE || p == CmpInst::FCMP_TRUE)
    return t.boolean(p == CmpInst::FCMP_TRUE);
  SMTExprRef a = fp(lhs), b = fp(rhs);
  SMTExprRef unordered = t.lor(s.mkFPIsNaN(a), s.mkFPIsNaN(b));
  if (p == CmpInst::FCMP_UNO || p == CmpInst::FCMP_ORD)
    return p == CmpInst::FCMP_UNO ? unordered : t.lnot(unordered);
  SMTExprRef r;
  switch (CmpInst::getOrderedPredicate(p)) {
  case CmpInst::FCMP_OEQ: r = s.mkFPEqual(a, b); break;
  case CmpInst::FCMP_ONE: r = t.lnot(s.mkFPEqual(a, b)); break;
  case CmpInst::FCMP_OGT: r = s.mkFPGt(a, b); break;
  case CmpInst::FCMP_OGE: r = s.mkFPGe(a, b); break;
  case CmpInst::FCMP_OLT: r = s.mkFPLt(a, b); break;
  case CmpInst::FCMP_OLE: r = s.mkFPLe(a, b); break;
  default: throw Unsupported("predicate " + std::string(CmpInst::getPredicateName(p)));
  }
  // Ordered: false on a NaN; unordered: true on one.
  return CmpInst::isOrdered(p) ? t.land(t.lnot(unordered), r) : t.lor(unordered, r);
}

SMTExprRef Encoder::castTo(unsigned opcode, const Value *op, Type *dst) {
  Type *src = op->getType();
  SMTExprRef a = value(op);
  switch (opcode) {
  case Instruction::FPToSI:
  case Instruction::FPToUI: { // toward zero, as C converts
    if (!dst->isIntegerTy())
      throw Unsupported("cast " + describe(dst));
    unsigned to = dst->getIntegerBitWidth();
    SMTExprRef r = opcode == Instruction::FPToSI ? s.mkFPToSBV(fp(op), to)
                                                 : s.mkFPToUBV(fp(op), to);
    return to == 1 ? t.eq(r, t.bv(1, 1)) : r;
  }
  case Instruction::SIToFP:
  case Instruction::UIToFP: {
    if (src->isIntegerTy(1))
      a = opcode == Instruction::SIToFP ? t.ite(a, t.bv(-1, 1), t.bv(0, 1)) : fromBool(a, 1);
    return bits(opcode == Instruction::SIToFP ? s.mkSBVToFP(a, fpSort(dst), rm(camada::RM::ROUND_TO_EVEN))
                                              : s.mkUBVToFP(a, fpSort(dst), rm(camada::RM::ROUND_TO_EVEN)));
  }
  case Instruction::FPExt:
  case Instruction::FPTrunc:
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
    SMTExprRef r = t.extract(a, to - 1, 0);
    return to == 1 ? t.eq(r, t.bv(1, 1)) : r;
  }
  default:
    throw Unsupported("cast " + std::string(Instruction::getOpcodeName(opcode)));
  }
}

SMTExprRef Encoder::toBool(SMTExprRef bv) {
  return t.lnot(t.eq(bv, t.bv(0, bv->getWidth())));
}

SMTExprRef Encoder::fromBool(SMTExprRef b, unsigned width) {
  return t.ite(b, t.bv(1, width), t.bv(0, width));
}

SMTExprRef Encoder::isTrue(const Value *v) {
  if (ieee(v->getType())) { // nonzero, as C reads a scalar: NaN is true
    SMTExprRef zero = fromBits(t.bv(0, v->getType()->getPrimitiveSizeInBits()), v->getType());
    return t.lnot(native() ? s.mkFPEqual(fp(v), zero)
                           : s.mkFPEqual(fp(v), s.mkBVToIEEEFP(zero, fpSort(v->getType()))));
  }
  SMTExprRef t = value(v);
  return v->getType()->isIntegerTy(1) ? t : toBool(t);
}

SMTExprRef Encoder::binary(unsigned opcode, const Value *lhs, const Value *rhs) {
  if (lhs->getType()->isFloatingPointTy()) {
    SMTExprRef a = fp(lhs), b = fp(rhs), even = rm(camada::RM::ROUND_TO_EVEN);
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
  SMTExprRef a = value(lhs), b = value(rhs);
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
  SMTExprRef r;
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
  SMTExprRef r;
  if (isa<BinaryOperator>(inst)) {
    r = binary(inst.getOpcode(), inst.getOperand(0), inst.getOperand(1));
  } else if (inst.getOpcode() == Instruction::FNeg) { // flips the sign bit, NaN or not
    SMTExprRef a = value(inst.getOperand(0));
    r = native() && ieee(inst.getType()) ? s.mkFPNeg(a)
                                         : t.bvxor(a, t.bv(APInt::getSignMask(a->getWidth())));
  } else if (auto *fc = dyn_cast<FCmpInst>(&inst)) {
    r = fcmp(fc->getPredicate(), fc->getOperand(0), fc->getOperand(1));
  } else if (auto *cmp = dyn_cast<ICmpInst>(&inst)) {
    SMTExprRef a = value(cmp->getOperand(0)), b = value(cmp->getOperand(1));
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
    SMTExprRef v = value(vec);
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
  SMTExprRef r;
  switch (id) {
  case Intrinsic::fabs:
  case Intrinsic::copysign: { // operations on the sign, NaN or not
    SMTExprRef a = value(arg(0));
    if (native() && ieee(arg(0)->getType())) { // x86_fp80 stays bits
      r = s.mkFPAbs(a);
      if (id == Intrinsic::copysign)
        r = t.ite(signBit(arg(1)), s.mkFPNeg(r), r);
      break;
    }
    SMTExprRef sign = t.bv(APInt::getSignMask(a->getWidth()));
    r = t.bvand(a, t.bvnot(sign));
    if (id == Intrinsic::copysign)
      r = t.bvor(r, t.bvand(value(arg(1)), sign));
    break;
  }
  case Intrinsic::sqrt: r = bits(s.mkFPSqrt(fp(arg(0)), rm(camada::RM::ROUND_TO_EVEN))); break;
  case Intrinsic::fma:
  case Intrinsic::fmuladd: // fused: one rounding, which fmuladd allows
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
    SMTExprRef a = fp(arg(0)), b = fp(arg(1));
    SMTExprRef first = id == Intrinsic::minnum ? s.mkFPLe(a, b) : s.mkFPGe(a, b);
    r = t.ite(s.mkFPIsNaN(a), value(arg(1)),
              t.ite(t.lor(s.mkFPIsNaN(b), first), value(arg(0)), value(arg(1))));
    break;
  }
  case Intrinsic::is_fpclass:
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
    SMTExprRef a = value(c.getArgOperand(0)), r = t.extract(a, 7, 0);
    for (unsigned lo = 8; lo < a->getWidth(); lo += 8)
      r = t.concat(r, t.extract(a, lo + 7, lo));
    bind(&c, r);
    return true;
  }
  SMTExprRef (Terms::*op)(const SMTExprRef &, const SMTExprRef &) = nullptr;
  SMTExprRef (Terms::*less)(const SMTExprRef &, const SMTExprRef &) = &Terms::slt;
  switch (f->getIntrinsicID()) { // InstCombine's min and max
  case Intrinsic::umin:
  case Intrinsic::umax: less = &Terms::ult; [[fallthrough]];
  case Intrinsic::smin:
  case Intrinsic::smax: {
    SMTExprRef a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
    bool min = f->getIntrinsicID() == Intrinsic::umin || f->getIntrinsicID() == Intrinsic::smin;
    bind(&c, t.ite((t.*less)(a, b), min ? a : b, min ? b : a));
    return true;
  }
  default: break;
  }
  SMTExprRef (Terms::*overflow)(const SMTExprRef &, const SMTExprRef &) = nullptr;
  switch (f->getIntrinsicID()) {
  case Intrinsic::sadd_with_overflow: op = &Terms::add; overflow = &Terms::saddo; break;
  case Intrinsic::uadd_with_overflow: op = &Terms::add; overflow = &Terms::uaddo; break;
  case Intrinsic::ssub_with_overflow: op = &Terms::sub; overflow = &Terms::ssubo; break;
  case Intrinsic::usub_with_overflow: op = &Terms::sub; overflow = &Terms::usubo; break;
  case Intrinsic::smul_with_overflow: op = &Terms::mul; overflow = &Terms::smulo; break;
  case Intrinsic::umul_with_overflow: op = &Terms::mul; overflow = &Terms::umulo; break;
  default: return false;
  }
  SMTExprRef a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
  // {result, overflow bit}: the overflow bit packs above the result.
  bind(&c, t.concat(fromBool((t.*overflow)(a, b), 1), (t.*op)(a, b)));
  return true;
}

} // namespace vereda::encode
