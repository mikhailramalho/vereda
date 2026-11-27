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

// The format of a supported floating-point type (IEEE interchange formats and
// x86_fp80, which differs only by its explicit integer bit); null for
// ppc_fp128.
static const fltSemantics *ieee(Type *ty) {
  if (!ty->isFloatingPointTy() || ty->isPPC_FP128Ty())
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
    return useNative(ty) ? fpSort(ty) : s.mkBVSort(ty->getPrimitiveSizeInBits());
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
  // A canonical x86_fp80 has 79 independent bits; bit 63 is 1 iff exp != 0.
  if (ty->isX86_FP80Ty())
    return ieee79ToFP80(t.symbol(id, s.mkBVSort(79)));
  return t.symbol(id, sort(ty));
}

Term Encoder::root(const Facts &facts, Term x) {
  for (const Fact *f; (f = facts.find(key(x))) && f->equal;)
    x = f->equal;
  return x;
}

std::optional<Terms::Bounds> Encoder::known(const Facts &facts, const Term &x) {
  const Fact *f = facts.find(key(x));
  for (const Fact *g; f && f->equal && (g = facts.find(key(f->equal)));)
    f = g;
  return f ? f->bounds : std::nullopt;
}

Term Encoder::assuming(Term e) {
  if (!facts.size() || t.constant(e))
    return e;
  return t.assuming(e, [&](const Term &x) { return known(facts, x); });
}

void Encoder::learn(const Term &cond, Facts &into) {
  // A term's facts go to the end of its chain, which stands for it.
  t.equalities(cond, true, [&](const Term &x, const Term &y) {
    Term rx = root(into, x), ry = root(into, y);
    if (t.constant(rx))
      std::swap(rx, ry);
    if (rx == ry || t.constant(rx)) // two constants: no execution takes the path
      return;
    if (const APInt *c = t.constant(ry)) {
      std::optional<Terms::Bounds> b;
      if (c->getBitWidth() <= 64)
        b = Terms::Bounds{c->getZExtValue(), c->getZExtValue(), c->getSExtValue(),
                          c->getSExtValue()};
      into.set(key(rx), {ry, b});
      return;
    }
    // Two terms: the one that is a leaf stands for both, with the bounds
    // both have; when neither is, only the bounds meet (narrow, below).
    if (!t.leaf(ry))
      std::swap(rx, ry);
    if (!t.leaf(ry))
      return;
    std::optional<Terms::Bounds> a = known(into, rx), b = known(into, ry);
    if (!b) {
      b = a;
    } else if (a) {
      Terms::Bounds both{std::max(a->ulo, b->ulo), std::min(a->uhi, b->uhi),
                         std::max(a->slo, b->slo), std::min(a->shi, b->shi)};
      if (both.ulo <= both.uhi && both.slo <= both.shi)
        b = both;
    }
    into.set(key(rx), {ry, std::nullopt});
    if (b)
      into.set(key(ry), {Term(), b});
  });
  t.narrow(cond, true, [&](const Term &x) { return known(into, x); },
           [&](const Term &x, const Terms::Bounds &b) {
             if (Term r = root(into, x); !t.constant(r))
               into.set(key(r), {Term(), b});
           });
}

std::optional<Encoder::Fact> Encoder::join(const Fact &a, const Fact &b) {
  Fact r{a.equal == b.equal ? a.equal : Term(), std::nullopt};
  if (a.bounds && b.bounds)
    r.bounds = Terms::Bounds{std::min(a.bounds->ulo, b.bounds->ulo),
                             std::max(a.bounds->uhi, b.bounds->uhi),
                             std::min(a.bounds->slo, b.bounds->slo),
                             std::max(a.bounds->shi, b.bounds->shi)};
  if (!r.equal && !r.bounds)
    return std::nullopt;
  return r;
}

Term Encoder::value(const Value *v) {
  if (auto it = env.find(v); it != env.end())
    return root(facts, it->second);
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

// x86_fp80 stores an explicit integer bit at bit 63 (0 when the 15-bit biased
// exponent is zero, 1 otherwise for canonical values); SMT-LIB's
// (_ FloatingPoint 15 64) omits it, giving a 79-bit IEEE interchange layout.
// Normalize x87 pseudo-denormals (exp == 0, bit 63 == 1) to exp == 1 (same
// numeric value, e.g. LDBL_MIN when frac == 0), and map unsupported x87
// encodings (exp != 0, bit 63 == 0: unnormal, pseudo-infinity, pseudo-NaN) to
// a quiet NaN, matching x87 hardware (#IA -> QNaN / unordered).
Term Encoder::fp80ToIEEE79(Term bv80) {
  Term sign = t.extract(bv80, 79, 79);
  Term exp = t.extract(bv80, 78, 64);
  Term intBit = t.extract(bv80, 63, 63);
  Term frac = t.extract(bv80, 62, 0);
  Term expZero = t.eq(exp, t.bv(0, 15));
  Term intZero = t.eq(intBit, t.bv(0, 1));
  Term pseudoDenorm = t.land(expZero, t.lnot(intZero));
  Term unsupported = t.land(t.lnot(expZero), intZero);
  Term ieeeExp =
      t.ite(unsupported, t.bvnot(t.bv(0, 15)), t.ite(pseudoDenorm, t.bv(1, 15), exp));
  Term qnanFrac = t.bv(APInt::getSignMask(63)); // bit 62 set (quiet NaN)
  Term ieeeFrac = t.ite(unsupported, qnanFrac, frac);
  return t.concat(t.concat(sign, ieeeExp), ieeeFrac);
}

Term Encoder::ieee79ToFP80(Term bv79) {
  Term signExp = t.extract(bv79, 78, 63);
  Term exp = t.extract(bv79, 77, 63);
  Term intBit = t.ite(t.eq(exp, t.bv(0, 15)), t.bv(0, 1), t.bv(1, 1));
  return t.concat(t.concat(signExp, intBit), t.extract(bv79, 62, 0));
}

SMTSortRef Encoder::fpSort(Type *ty) { return fpSort(ty, fpEncodingOf(ty)); }

SMTSortRef Encoder::fpSort(Type *ty, camada::FPEncoding encoding) {
  const fltSemantics *sem = ieee(ty);
  if (!sem)
    throw Unsupported("type " + describe(ty));
  unsigned precision = APFloat::semanticsPrecision(*sem); // with the hidden/integer bit
  unsigned expBits = ty->isX86_FP80Ty() ? 15 : APFloat::semanticsSizeInBits(*sem) - precision;
  return s.mkFPSort(expBits, precision - 1, encoding);
}

camada::SMTExprRef Encoder::fp(const Value *v) { return fp(value(v), v->getType()); }

camada::SMTExprRef Encoder::fp(Term x, Type *ty) {
  if (useNative(ty))
    return t.solverTerm(x);
  if (ty->isX86_FP80Ty())
    x = fp80ToIEEE79(x);
  return s.mkBVToIEEEFP(t.solverTerm(x), fpSort(ty));
}

Term Encoder::fpBinary(unsigned opcode, Term a, Term b, Type *ty) {
  camada::SMTExprRef x = fp(a, ty), y = fp(b, ty), even = rm(camada::RM::ROUND_TO_EVEN, ty);
  switch (opcode) {
  case Instruction::FAdd: return bits(s.mkFPAdd(x, y, even), ty);
  case Instruction::FSub: return bits(s.mkFPSub(x, y, even), ty);
  default: throw Unsupported(std::string("instruction ") + Instruction::getOpcodeName(opcode));
  }
}

Term Encoder::fpMinMax(unsigned id, Term a, Term b, Type *ty) {
  bool min = id == Intrinsic::minnum || id == Intrinsic::minimum;
  camada::SMTExprRef x = fp(a, ty), y = fp(b, ty);
  Term first = t.fromSolver(min ? s.mkFPLe(x, y) : s.mkFPGe(x, y));
  Term nanA = t.fromSolver(s.mkFPIsNaN(x)), nanB = t.fromSolver(s.mkFPIsNaN(y));
  if (id == Intrinsic::minnum || id == Intrinsic::maxnum)
    return t.ite(nanA, b, t.ite(t.lor(nanB, first), a, b));
  // Of two zeros, minimum takes a when a is -0, maximum when a is +0.
  Term zeros = t.land(t.fromSolver(s.mkFPIsZero(x)), t.fromSolver(s.mkFPIsZero(y)));
  Term negA = signBit(a, ty);
  Term takeA = t.ite(zeros, min ? negA : t.lnot(negA), first);
  return t.ite(nanA, a, t.ite(nanB, b, t.ite(takeA, a, b)));
}

Term Encoder::bits(const camada::SMTExprRef &f, Type *ty) {
  if (useNative(ty))
    return t.fromSolver(f);
  Term b = t.fromSolver(s.mkIEEEFPToBV(f));
  return ty->isX86_FP80Ty() ? ieee79ToFP80(b) : b;
}

Term Encoder::toBits(Term v, Type *ty) {
  if (!useNative(ty) || !ieee(ty))
    return v;
  const fltSemantics *sem = ieee(ty);
  // A constant's bits, except a NaN's, which are any NaN's as for every NaN term.
  if (auto it = floatBits.find(v.id());
      it != floatBits.end() && !APFloat(*sem, it->second).isNaN())
    return t.bv(it->second);
  Term b = t.fromSolver(s.mkIEEEFPToBV(t.solverTerm(v)));
  if (ty->isX86_FP80Ty())
    b = ieee79ToFP80(b);
  bitsFloat.try_emplace({b.id(), sem}, v);
  return b;
}

Term Encoder::fromBits(Term b, Type *ty) {
  if (!useNative(ty) || !ieee(ty))
    return b;
  const fltSemantics *sem = ieee(ty);
  if (auto it = bitsFloat.find({b.id(), sem}); it != bitsFloat.end())
    return it->second;
  const APInt *c = t.constant(b);
  Term ieeeBits = ty->isX86_FP80Ty() ? fp80ToIEEE79(b) : b;
  if (!c)
    return t.fromSolver(s.mkBVToIEEEFP(t.solverTerm(ieeeBits), fpSort(ty)));
  Term &f = floatTerms[{sem, *c}];
  if (!f) {
    f = t.fromSolver(s.mkBVToIEEEFP(t.solverTerm(ieeeBits), fpSort(ty)));
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
  if (!useNative(v->getType()))
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

Term Encoder::signBit(const Value *v) { return signBit(value(v), v->getType()); }

Term Encoder::signBit(Term x, Type *ty) {
  Term b = toBits(x, ty);
  unsigned w = b.width();
  return t.eq(t.extract(b, w - 1, w - 1), t.bv(1, 1));
}

camada::SMTExprRef Encoder::rm(camada::RM mode, Type *ty) {
  return s.mkRM(mode, fpEncodingOf(ty));
}

Term Encoder::fpClass(const Term &x, Type *ty, unsigned mask) {
  if (!ieee(ty))
    throw Unsupported("type " + describe(ty));
  if (useNative(ty)) {
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
  Term b = ty->isX86_FP80Ty() ? fp80ToIEEE79(x) : x;
  unsigned w = b.width(), sig = APFloat::semanticsPrecision(sem) - 1;
  Term exp = t.extract(b, w - 2, sig), mant = t.extract(b, sig - 1, 0);
  Term neg = t.eq(t.extract(b, w - 1, w - 1), t.bv(1, 1));
  Term top = t.eq(exp, t.bvnot(t.bv(0, w - 1 - sig)));
  Term low = t.eq(exp, t.bv(0, w - 1 - sig));
  Term noMant = t.eq(mant, t.bv(0, sig));
  Term nan = t.land(top, t.lnot(noMant));
  Term quiet = t.eq(t.extract(b, sig - 1, sig - 1), t.bv(1, 1));
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
  // SMT-LIB's fp.eq, fp.lt, fp.leq, fp.gt and fp.geq all return false on a
  // NaN, so ordered predicates hold directly and unordered ones are their
  // negations; only UNO, ORD, ONE and UEQ need explicit NaN checks.
  switch (p) {
  case CmpInst::FCMP_OEQ: return t.fromSolver(s.mkFPEqual(a, b));
  case CmpInst::FCMP_OGT: return t.fromSolver(s.mkFPGt(a, b));
  case CmpInst::FCMP_OGE: return t.fromSolver(s.mkFPGe(a, b));
  case CmpInst::FCMP_OLT: return t.fromSolver(s.mkFPLt(a, b));
  case CmpInst::FCMP_OLE: return t.fromSolver(s.mkFPLe(a, b));
  case CmpInst::FCMP_UNE: return t.lnot(t.fromSolver(s.mkFPEqual(a, b)));
  case CmpInst::FCMP_UGT: return t.lnot(t.fromSolver(s.mkFPLe(a, b)));
  case CmpInst::FCMP_UGE: return t.lnot(t.fromSolver(s.mkFPLt(a, b)));
  case CmpInst::FCMP_ULT: return t.lnot(t.fromSolver(s.mkFPGe(a, b)));
  case CmpInst::FCMP_ULE: return t.lnot(t.fromSolver(s.mkFPGt(a, b)));
  case CmpInst::FCMP_UNO:
  case CmpInst::FCMP_ORD:
  case CmpInst::FCMP_ONE:
  case CmpInst::FCMP_UEQ: {
    Term unordered = t.lor(t.fromSolver(s.mkFPIsNaN(a)), t.fromSolver(s.mkFPIsNaN(b)));
    if (p == CmpInst::FCMP_UNO)
      return unordered;
    if (p == CmpInst::FCMP_ORD)
      return t.lnot(unordered);
    Term eq = t.fromSolver(s.mkFPEqual(a, b));
    return p == CmpInst::FCMP_ONE ? t.land(t.lnot(unordered), t.lnot(eq))
                                  : t.lor(unordered, eq);
  }
  default:
    throw Unsupported("predicate " + std::string(CmpInst::getPredicateName(p)));
  }
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
    return bits(opcode == Instruction::SIToFP
                    ? s.mkSBVToFP(x, fpSort(dst), rm(camada::RM::ROUND_TO_EVEN, dst))
                    : s.mkUBVToFP(x, fpSort(dst), rm(camada::RM::ROUND_TO_EVEN, dst)),
                dst);
  }
  case Instruction::FPExt:
  case Instruction::FPTrunc:
    if (auto c = constFloat(op); c && ieee(dst)) {
      bool loses;
      c->convert(*ieee(dst), APFloat::rmNearestTiesToEven, &loses);
      if (Term r = folded(*c, dst))
        return r;
    }
    if (useNative(src) != useNative(dst)) {
      // Converting between a native FP format and a BV-encoded one (x86_fp80 /
      // bfloat): do the conversion in camada's BV FP encoding so both sides
      // share one encoding.
      Term srcBits = toBits(a, src);
      if (src->isX86_FP80Ty())
        srcBits = fp80ToIEEE79(srcBits);
      camada::SMTExprRef from =
          s.mkBVToIEEEFP(t.solverTerm(srcBits), fpSort(src, camada::FPEncoding::BV));
      camada::SMTExprRef conv =
          s.mkFPToFP(from, fpSort(dst, camada::FPEncoding::BV),
                     s.mkRM(camada::RM::ROUND_TO_EVEN, camada::FPEncoding::BV));
      Term dstBits = t.fromSolver(s.mkIEEEFPToBV(conv));
      if (dst->isX86_FP80Ty())
        dstBits = ieee79ToFP80(dstBits);
      return fromBits(dstBits, dst);
    }
    return bits(s.mkFPToFP(fp(op), fpSort(dst), rm(camada::RM::ROUND_TO_EVEN, dst)), dst);
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
  case Instruction::PtrToAddr: // a ptrtoint that does not expose the pointer
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
    Type *ty = v->getType();
    Term zero = fromBits(t.bv(0, ty->getPrimitiveSizeInBits()), ty);
    camada::SMTExprRef f = fp(v), z = t.solverTerm(zero);
    if (!useNative(ty)) {
      if (ty->isX86_FP80Ty())
        zero = fp80ToIEEE79(zero);
      z = s.mkBVToIEEEFP(t.solverTerm(zero), fpSort(ty));
    }
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
    Type *ty = lhs->getType();
    camada::SMTExprRef a = fp(lhs), b = fp(rhs), even = rm(camada::RM::ROUND_TO_EVEN, ty);
    switch (opcode) {
    case Instruction::FAdd: return bits(s.mkFPAdd(a, b, even), ty);
    case Instruction::FSub: return bits(s.mkFPSub(a, b, even), ty);
    case Instruction::FMul: return bits(s.mkFPMul(a, b, even), ty);
    case Instruction::FDiv: return bits(s.mkFPDiv(a, b, even), ty);
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
      r = useNative(inst.getType()) && ieee(inst.getType())
              ? t.fromSolver(s.mkFPNeg(t.solverTerm(a)))
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
    // The path's bounds settle it here; the term itself stays as it is
    // for other paths.
    r = assuming(r);
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
    Type *ty = arg(0)->getType();
    return bits(s.mkFPToIntegral(fp(arg(0)), rm(mode, ty)), ty);
  };
  Term r;
  switch (id) {
  case Intrinsic::fabs:
  case Intrinsic::copysign: { // operations on the sign, NaN or not
    Term a = value(arg(0));
    if (useNative(arg(0)->getType()) && ieee(arg(0)->getType())) {
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
      r = bits(s.mkFPSqrt(fp(arg(0)), rm(camada::RM::ROUND_TO_EVEN, ty)), ty);
    break;
  }
  case Intrinsic::fma:
  case Intrinsic::fmuladd: { // fused: one rounding, which fmuladd allows
    Type *ty = arg(0)->getType();
    if (auto x = constFloat(arg(0)), y = constFloat(arg(1)), z = constFloat(arg(2)); x && y && z) {
      x->fusedMultiplyAdd(*y, *z, APFloat::rmNearestTiesToEven);
      if ((r = folded(*x, ty)))
        break;
    }
    r = bits(s.mkFPFMA(fp(arg(0)), fp(arg(1)), fp(arg(2)), rm(camada::RM::ROUND_TO_EVEN, ty)), ty);
    break;
  }
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
    r = fpMinMax(id, value(arg(0)), value(arg(1)), arg(0)->getType());
    break;
  }
  case Intrinsic::is_fpclass:
    // Natively either NaN class is any NaN, as fpClass has it; with fp2bv
    // fpClass reads the constant's bits and folds.
    if (auto c = constFloat(arg(0)); c && useNative(arg(0)->getType())) {
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
  if (f->getIntrinsicID() == Intrinsic::ctlz || f->getIntrinsicID() == Intrinsic::cttz) {
    if (!c.getType()->isIntegerTy())
      return false;
    unsigned w = c.getType()->getIntegerBitWidth();
    Term a = value(c.getArgOperand(0));
    if (w == 1)
      a = fromBool(a, 1);
    Term r = t.bv(w, w);
    bool leading = f->getIntrinsicID() == Intrinsic::ctlz;
    // Later nonzero bits override earlier ones: highest for ctlz, lowest for cttz.
    for (unsigned i = 0; i < w; ++i) {
      unsigned bit = leading ? i : w - 1 - i;
      r = t.ite(t.eq(t.extract(a, bit, bit), t.bv(1, 1)),
                t.bv(leading ? w - 1 - bit : bit, w), r);
    }
    if (w == 1)
      r = toBool(r);
    // An unchecked poison-producing zero is unconstrained, like other poison.
    if (cast<ConstantInt>(c.getArgOperand(1))->isOne())
      r = t.ite(t.eq(a, t.bv(0, w)), fresh(c.getType(), "count_zero"), r);
    bind(&c, r);
    return true;
  }
  if (f->getIntrinsicID() == Intrinsic::ptrmask && c.getType()->isPointerTy()) {
    bind(&c, mem.mask(value(c.getArgOperand(0)),
                      resize(value(c.getArgOperand(1)), mem.pointerBits(), false)));
    return true;
  }
  if (f->getIntrinsicID() == Intrinsic::bswap) { // the low byte goes highest
    Term a = value(c.getArgOperand(0)), r = t.extract(a, 7, 0);
    for (unsigned lo = 8; lo < a.width(); lo += 8)
      r = t.concat(r, t.extract(a, lo + 7, lo));
    bind(&c, r);
    return true;
  }
  if (f->getIntrinsicID() == Intrinsic::ctpop) {
    if (!c.getType()->isIntegerTy())
      return false;
    unsigned w = c.getType()->getIntegerBitWidth();
    Term a = value(c.getArgOperand(0));
    if (w == 1) {
      bind(&c, a);
      return true;
    }
    if (const APInt *v = t.constant(a)) {
      bind(&c, t.bv(APInt(w, v->popcount())));
      return true;
    }
    // Pairwise adder tree: each level widens by 1 bit, so no sum overflows
    // and the final width is 1 + ceil(log2(w)) <= w for all w >= 2.
    SmallVector<Term, 64> cur;
    cur.reserve(w);
    for (unsigned i = 0; i < w; ++i)
      cur.push_back(t.extract(a, i, i));
    while (cur.size() > 1) {
      SmallVector<Term, 32> next;
      next.reserve((cur.size() + 1) / 2);
      for (size_t i = 0; i + 1 < cur.size(); i += 2)
        next.push_back(t.add(t.zext(cur[i], 1), t.zext(cur[i + 1], 1)));
      if (cur.size() % 2 == 1)
        next.push_back(t.zext(cur.back(), 1));
      cur = std::move(next);
    }
    Term r = cur[0];
    if (r.width() < w)
      r = t.zext(r, w - r.width());
    bind(&c, r);
    return true;
  }
  if (f->getIntrinsicID() == Intrinsic::bitreverse) {
    if (!c.getType()->isIntegerTy())
      return false;
    unsigned w = c.getType()->getIntegerBitWidth();
    Term a = value(c.getArgOperand(0));
    if (w == 1) {
      bind(&c, a);
      return true;
    }
    if (const APInt *v = t.constant(a)) {
      bind(&c, t.bv(v->reverseBits()));
      return true;
    }
    Term r = t.extract(a, 0, 0);
    for (unsigned i = 1; i < w; ++i)
      r = t.concat(r, t.extract(a, i, i));
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
  switch (f->getIntrinsicID()) { // InstCombine's min, max and three-way compare
  case Intrinsic::umin:
  case Intrinsic::umax: less = &Terms::ult; [[fallthrough]];
  case Intrinsic::smin:
  case Intrinsic::smax: {
    Term a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
    bool min = f->getIntrinsicID() == Intrinsic::umin || f->getIntrinsicID() == Intrinsic::smin;
    bind(&c, t.ite((t.*less)(a, b), min ? a : b, min ? b : a));
    return true;
  }
  case Intrinsic::ucmp: less = &Terms::ult; [[fallthrough]];
  case Intrinsic::scmp: { // -1, 0 or 1, in the call's own width
    Term a = value(c.getArgOperand(0)), b = value(c.getArgOperand(1));
    unsigned w = c.getType()->getIntegerBitWidth();
    bind(&c, t.ite((t.*less)(a, b), t.bv(-1, w),
                   t.ite((t.*less)(b, a), t.bv(1, w), t.bv(0, w))));
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
  bind(&c, t.concat(fromBool(assuming((t.*overflow)(a, b)), 1), (t.*op)(a, b)));
  return true;
}

} // namespace vereda::encode
