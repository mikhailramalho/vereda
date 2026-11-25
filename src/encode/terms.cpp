#include "encode/terms.h"

#include <algorithm>
#include <numeric>
#include <llvm/ADT/Hashing.h>

using llvm::APInt;

namespace vereda::encode {

using Ref = Terms::Ref;

static const APInt False(1, 0), True(1, 1);

Terms::Terms(camada::SMTSolver &s) : s(s), bools{Ref(s.mkBool(false)), Ref(s.mkBool(true))} {}

uint32_t Terms::hashKey(const Key &k) {
  return static_cast<uint32_t>(
      llvm::hash_combine(static_cast<uint8_t>(k.op), k.a, k.b, k.c, k.x, k.y));
}

Ref Terms::memoFind(const Key &k) const {
  if (memoBuckets.empty())
    return {};
  uint32_t h = hashKey(k);
  size_t mask = memoBuckets.size() - 1;
  for (size_t i = h & mask;; i = (i + 1) & mask) {
    const MemoBucket &b = memoBuckets[i];
    if (b.idx == MemoBucket::Empty)
      return {};
    if (b.hash == h && matches(entries[b.idx].n, k))
      return entries[b.idx].self;
  }
}

void Terms::memoGrow() {
  size_t newSize = memoBuckets.empty() ? 64 : memoBuckets.size() * 2;
  std::vector<MemoBucket> next(newSize);
  size_t mask = newSize - 1;
  for (const MemoBucket &b : memoBuckets) {
    if (b.idx == MemoBucket::Empty)
      continue;
    size_t i = b.hash & mask;
    while (next[i].idx != MemoBucket::Empty)
      i = (i + 1) & mask;
    next[i] = b;
  }
  memoBuckets.swap(next);
}

void Terms::memoInsert(uint32_t idx, uint32_t h) {
  if ((memoCount + 1) * 3 >= memoBuckets.size() * 2)
    memoGrow();
  size_t mask = memoBuckets.size() - 1;
  size_t i = h & mask;
  while (memoBuckets[i].idx != MemoBucket::Empty)
    i = (i + 1) & mask;
  memoBuckets[i] = {idx, h};
  ++memoCount;
}

const APInt *Terms::constant(const Ref &e) const {
  if (const Term::Constant *c = e.entry())
    return &c->value;
  return isTrue(e) ? &True : isFalse(e) ? &False : nullptr;
}

const Terms::Node *Terms::node(const Ref &e) const {
  if (!e || e.entry())
    return nullptr;
  auto it = nodes.find(e.id());
  return it == nodes.end() ? nullptr : &entries[it->second].n;
}

Ref Terms::bv(const APInt &v) {
  auto [it, fresh] = bvs.try_emplace(v);
  if (fresh)
    it->second = &constants.emplace_back(Term::Constant{v, {}});
  return Ref(it->second);
}

Ref Terms::boolean(bool b) { return bools[b]; }

camada::SMTExprRef Terms::solverTerm(const Ref &e) {
  const Term::Constant *c = e.entry();
  if (!c)
    return e.ref();
  if (!c->term) {
    // Up to 64 bits, camada hands the value to the solver as an integer;
    // a string of digits would be parsed back, twice in Bitwuzla.
    const APInt &v = c->value;
    unsigned w = v.getBitWidth();
    if (w <= 64) {
      c->term = s.mkBVFromDec(static_cast<int64_t>(v.getZExtValue()), w);
    } else {
      std::string bits(w, '0');
      for (unsigned i = 0; i < w; ++i)
        if (v[i])
          bits[w - 1 - i] = '1';
      c->term = s.mkBVFromBin(bits, w);
    }
    if (!walkDone)
      made.try_emplace(c->term.get(), e);
  }
  return c->term;
}

void Terms::releaseWalkTables() {
  decltype(entries)().swap(entries);
  decltype(memoBuckets)().swap(memoBuckets);
  memoCount = 0;
  decltype(nodes)().swap(nodes);
  decltype(bvs)().swap(bvs);
  decltype(made)().swap(made);
  decltype(implied)().swap(implied);
  decltype(arith)().swap(arith);
  decltype(filled)().swap(filled);
  decltype(lambdas)().swap(lambdas);
  decltype(under)().swap(under);
  decltype(reduced)().swap(reduced);
  decltype(selects)().swap(selects);
  decltype(scratch)().swap(scratch);
  decltype(bounds)().swap(bounds);
  decltype(ranges)().swap(ranges);
  decltype(freeOf)().swap(freeOf);
  walkDone = true;
}

Ref Terms::fromSolver(const camada::SMTExprRef &e) const {
  if (!e)
    return {};
  auto it = made.find(e.get());
  return it == made.end() ? Ref(e) : it->second;
}

bool Terms::entails(const Ref &guard, const Ref &lit) { return entails(guard, lit, 0); }

bool Terms::entails(const Ref &guard, const Ref &lit, unsigned depth) {
  if (guard == lit || isTrue(lit) || isFalse(guard))
    return true;
  const Node *n = node(guard);
  // Deep guards give up, which only loses a fold.
  if (!n || (n->op != Op::And && n->op != Op::Or) || depth > 1000)
    return false;
  auto [it, fresh] = implied.try_emplace({guard.id(), lit.id()}, false);
  if (!fresh)
    return it->second;
  Ref a = n->a, b = n->b;
  bool r = n->op == Op::And ? entails(a, lit, depth + 1) || entails(b, lit, depth + 1)
                            : entails(a, lit, depth + 1) && entails(b, lit, depth + 1);
  implied[{guard.id(), lit.id()}] = r; // the map may have rehashed
  return r;
}

void Terms::equalities(const Ref &cond, bool holds,
                       llvm::function_ref<void(const Ref &, const Ref &)> f) const {
  // Conditions share subterms, so a walk without a bound can take
  // exponential time; equalities sit near the top.
  unsigned budget = 64;
  literals(cond, holds, [&](const Node &n, bool h) {
    if (n.op != Op::Eq || !h || n.a.isBool())
      return;
    if (constant(n.b) && !constant(n.a))
      f(n.a, n.b);
    else if (constant(n.a) && !constant(n.b))
      f(n.b, n.a);
  }, budget);
}

void Terms::literals(const Ref &cond, bool holds, llvm::function_ref<void(const Node &, bool)> f,
                     unsigned &budget) const {
  const Node *n = node(cond);
  if (!n || !budget)
    return;
  --budget;
  switch (n->op) {
  case Op::Not: literals(n->a, !holds, f, budget); return;
  case Op::And:
  case Op::Or:
    if ((n->op == Op::And) == holds) { // both sides hold, or both fail
      literals(n->a, holds, f, budget);
      literals(n->b, holds, f, budget);
    }
    return;
  case Op::Ite: // a Bool ite with a constant arm: a && b, a || b
    if (constant(n->b) && constant(n->b)->isZero() == holds) {
      literals(n->a, false, f, budget);
      literals(n->c, holds, f, budget);
    } else if (constant(n->c) && constant(n->c)->isZero() == holds) {
      literals(n->a, true, f, budget);
      literals(n->b, holds, f, budget);
    }
    return;
  default: f(*n, holds); return;
  }
}

bool Terms::split(const Ref &e, Ref &c, Ref &x, Ref &y) const {
  return choice(e, c, x, y) && constant(x) && constant(y);
}

bool Terms::choice(const Ref &e, Ref &c, Ref &x, Ref &y) const {
  const Node *n = node(e);
  if (!n || n->op != Op::Ite)
    return false;
  c = n->a;
  x = n->b;
  y = n->c;
  return true;
}

bool Terms::stored(const Ref &e, Ref &a, Ref &i, Ref &v) const {
  const Node *n = node(e);
  if (!n || n->op != Op::Store)
    return false;
  a = n->a;
  i = n->b;
  v = n->c;
  return true;
}

Ref Terms::filledWith(const Ref &e) const {
  auto it = filled.find(e.id());
  return it == filled.end() ? Ref() : it->second.second;
}

Ref Terms::make(Node n) {
  Key k{n.op, n.a.id(), n.b.id(), n.c.id(), n.x, n.y};
  uint32_t h = hashKey(k);
  if (!memoBuckets.empty()) {
    size_t mask = memoBuckets.size() - 1;
    for (size_t i = h & mask;; i = (i + 1) & mask) {
      const MemoBucket &b = memoBuckets[i];
      if (b.idx == MemoBucket::Empty)
        break;
      if (b.hash == h && matches(entries[b.idx].n, k))
        return entries[b.idx].self;
    }
  }
  Ref r(build(n));
  if (!lambdas.empty() && (n.op == Op::Store || n.op == Op::Ite)) {
    std::vector<Ref> reads;
    auto add = [&](const Ref &l) {
      if (std::none_of(reads.begin(), reads.end(), [&](const Ref &x) { return x == l; }))
        reads.push_back(l);
    };
    auto through = [&](const Ref &array) {
      if (lambdas.count(array.id()))
        add(array);
      else if (auto u = under.find(array.id()); u != under.end())
        for (const Ref &l : u->second)
          add(l);
    };
    if (n.op == Op::Store)
      through(n.a);
    else
      through(n.b), through(n.c);
    if (!reads.empty())
      under.emplace(r.id(), std::move(reads));
  }
  uint32_t idx = static_cast<uint32_t>(entries.size());
  entries.push_back(Entry{std::move(n), r});
  memoInsert(idx, h);
  nodes.try_emplace(r.id(), idx);
  return r;
}

camada::SMTExprRef Terms::build(const Node &n) {
  camada::SMTExprRef a = solverTerm(n.a), b = solverTerm(n.b), c = solverTerm(n.c);
  switch (n.op) {
  case Op::Add: return s.mkBVAdd(a, b);
  case Op::Sub: return s.mkBVSub(a, b);
  case Op::Mul: return s.mkBVMul(a, b);
  case Op::UDiv: return s.mkBVUDiv(a, b);
  case Op::SDiv: return s.mkBVSDiv(a, b);
  case Op::URem: return s.mkBVURem(a, b);
  case Op::SRem: return s.mkBVSRem(a, b);
  case Op::Shl: return s.mkBVShl(a, b);
  case Op::LShr: return s.mkBVLshr(a, b);
  case Op::AShr: return s.mkBVAshr(a, b);
  case Op::BVAnd: return s.mkBVAnd(a, b);
  case Op::BVOr: return s.mkBVOr(a, b);
  case Op::BVXor: return s.mkBVXor(a, b);
  case Op::BVNot: return s.mkBVNot(a);
  case Op::Concat: return s.mkBVConcat(a, b);
  case Op::Extract: return s.mkBVExtract(n.x, n.y, a);
  case Op::ZExt: return s.mkBVZeroExt(a, n.x);
  case Op::SExt: return s.mkBVSignExt(a, n.x);
  case Op::Eq: return s.mkEqual(a, b);
  case Op::Ult: return s.mkBVUlt(a, b);
  case Op::Ule: return s.mkBVUle(a, b);
  case Op::Slt: return s.mkBVSlt(a, b);
  case Op::Sle: return s.mkBVSle(a, b);
  case Op::And: return s.mkAnd(a, b);
  case Op::Or: return s.mkOr(a, b);
  case Op::Xor: return s.mkXor(a, b);
  case Op::Not: return s.mkNot(a);
  case Op::Ite: return s.mkIte(a, b, c);
  case Op::UAddO: return s.mkBVUAddOverflow(a, b);
  case Op::SAddO: return s.mkBVSAddOverflow(a, b);
  case Op::USubO: return s.mkBVUSubOverflow(a, b);
  case Op::SSubO: return s.mkBVSSubOverflow(a, b);
  case Op::UMulO: return s.mkBVUMulOverflow(a, b);
  case Op::SMulO: return s.mkBVSMulOverflow(a, b);
  case Op::Select: return s.mkArraySelect(a, b);
  case Op::Store: return s.mkArrayStore(a, b, c);
  }
  llvm_unreachable("unknown op");
}

// Binary bitvector operations fold when both sides are constant.
#define FOLD2(fn)                                                                    \
  const APInt *ca = constant(a), *cb = constant(b);                                  \
  if (ca && cb)                                                                      \
    return fn;

Ref Terms::add(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca + *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return add(x, y); }))
    return r;
  if (cb && cb->isZero()) return a;
  if (ca && ca->isZero()) return b;
  return make({Op::Add, a, b});
}

Ref Terms::sub(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca - *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return sub(x, y); }))
    return r;
  if (cb && cb->isZero()) return a;
  if (a == b) return bv(0, a.width());
  if (const Node *n = node(a); n && n->op == Op::Add) { // (x + y) - x
    if (n->a == b) return n->b;
    if (n->b == b) return n->a;
  }
  return make({Op::Sub, a, b});
}

Ref Terms::mul(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca * *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return mul(x, y); }))
    return r;
  if ((ca && ca->isZero()) || (cb && cb->isZero())) return bv(0, a.width());
  if (cb && cb->isOne()) return a;
  if (ca && ca->isOne()) return b;
  return make({Op::Mul, a, b});
}

// Division by zero is left to the solver, whose semantics differ from APInt's.
Ref Terms::udiv(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb && !cb->isZero()) return bv(ca->udiv(*cb));
  // x * c / c is x when x * c does not wrap.
  if (const Node *n = node(a); n && n->op == Op::Mul && cb && !cb->isZero() && range(a))
    if (const APInt *c = constant(n->b); c && *c == *cb)
      return n->a;
  return make({Op::UDiv, a, b});
}
Ref Terms::sdiv(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb && !cb->isZero()) return bv(ca->sdiv(*cb));
  return make({Op::SDiv, a, b});
}
Ref Terms::urem(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb && !cb->isZero()) return bv(ca->urem(*cb));
  if (const Node *n = node(a); n && n->op == Op::Mul && cb && !cb->isZero() && range(a))
    if (const APInt *c = constant(n->b); c && *c == *cb)
      return bv(0, a.width());
  return make({Op::URem, a, b});
}
Ref Terms::srem(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb && !cb->isZero()) return bv(ca->srem(*cb));
  return make({Op::SRem, a, b});
}

// APInt caps the shift amount at the width, which matches SMT-LIB.
Ref Terms::shl(const Ref &a, const Ref &b) {
  FOLD2(bv(ca->shl(*cb)))
  if (cb && cb->isZero()) return a;
  return make({Op::Shl, a, b});
}
Ref Terms::lshr(const Ref &a, const Ref &b) {
  FOLD2(bv(ca->lshr(*cb)))
  if (cb && cb->isZero()) return a;
  return make({Op::LShr, a, b});
}
Ref Terms::ashr(const Ref &a, const Ref &b) {
  FOLD2(bv(ca->ashr(*cb)))
  if (cb && cb->isZero()) return a;
  return make({Op::AShr, a, b});
}

Ref Terms::bvand(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca & *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return bvand(x, y); }))
    return r;
  if ((ca && ca->isZero()) || (cb && cb->isZero())) return bv(0, a.width());
  if (cb && cb->isAllOnes()) return a;
  if (ca && ca->isAllOnes()) return b;
  if (a == b) return a;
  return make({Op::BVAnd, a, b});
}
Ref Terms::bvor(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca | *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return bvor(x, y); }))
    return r;
  if (cb && cb->isZero()) return a;
  if (ca && ca->isZero()) return b;
  if (a == b) return a;
  return make({Op::BVOr, a, b});
}
Ref Terms::bvxor(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca ^ *cb))
  if (cb && cb->isZero()) return a;
  if (ca && ca->isZero()) return b;
  if (a == b) return bv(0, a.width());
  return make({Op::BVXor, a, b});
}
Ref Terms::bvnot(const Ref &a) {
  if (const APInt *ca = constant(a)) return bv(~*ca);
  if (const Node *n = node(a); n && n->op == Op::BVNot) return n->a;
  return make({Op::BVNot, a});
}

Ref Terms::concat(const Ref &hi, const Ref &lo) {
  const APInt *ch = constant(hi), *cl = constant(lo);
  if (ch && cl) return bv(ch->concat(*cl));
  if (Ref r = lift(hi, lo, [this](const Ref &x, const Ref &y) { return concat(x, y); }))
    return r;
  const Node *nh = node(hi), *nl = node(lo);
  // Bytes merged under the same conditions join back into one word, so a
  // merged global reads as ite(c, v, w), not as four byte-wise ites.
  // The result is remembered: a merged global's ite chain grows by one
  // level per join, and every load would otherwise walk all of it.
  // Past 1000 levels the join stops, which only loses a fold and
  // keeps the recursion off the end of the stack.
  if (nh && nl && nh->op == Op::Ite && nl->op == Op::Ite && nh->a == nl->a &&
      joinDepth < 1000) {
    Key k{Op::Concat, hi.id(), lo.id(), nullptr, 0, 0};
    if (Ref cached = memoFind(k))
      return cached;
    Ref c = nh->a, ha = nh->b, hb = nh->c, la = nl->b, lb = nl->c;
    ++joinDepth;
    Ref r = ite(c, concat(ha, la), concat(hb, lb));
    --joinDepth;
    if (!memoFind(k)) {
      uint32_t idx = static_cast<uint32_t>(entries.size());
      entries.push_back(Entry{Node{Op::Concat, hi, lo}, r});
      memoInsert(idx, hashKey(k));
    }
    return r;
  }
  // y[w-1:k] ++ 0^k is y when y's low k bits are zero: an offset made
  // into an element index and back.
  if (nh && nh->op == Op::Extract && cl && cl->isZero() && nh->y == lo.width() &&
      nh->x + 1 == nh->a.width() && zeros(nh->a) >= nh->y)
    return nh->a;
  // x[h:m+1] ++ x[m:l] is x[h:l]: bytes read back from one stored value.
  if (nh && nl && nh->op == Op::Extract && nl->op == Op::Extract &&
      nh->a == nl->a && nh->y == nl->x + 1)
    return extract(nh->a, nh->x, nl->y);
  return make({Op::Concat, hi, lo});
}

Ref Terms::extract(const Ref &a, unsigned hi, unsigned lo) {
  if (lo == 0 && hi + 1 == a.width()) return a;
  if (const APInt *ca = constant(a)) return bv(ca->extractBits(hi - lo + 1, lo));
  if (Ref r = lift(a, [&](const Ref &x) { return extract(x, hi, lo); }))
    return r;
  if (const Node *n = node(a)) {
    switch (n->op) {
    case Op::Extract:
      return extract(n->a, hi + n->y, lo + n->y);
    case Op::Concat: {
      unsigned w = n->b.width();
      if (hi < w) return extract(n->b, hi, lo);
      if (lo >= w) return extract(n->a, hi - w, lo - w);
      break;
    }
    case Op::ZExt:
    case Op::SExt:
      if (hi < n->a.width()) return extract(n->a, hi, lo);
      break;
    default:
      break;
    }
  }
  return make({Op::Extract, a, {}, {}, hi, lo});
}

Ref Terms::zext(const Ref &a, unsigned n) {
  if (!n) return a;
  if (const APInt *ca = constant(a)) return bv(ca->zext(ca->getBitWidth() + n));
  if (Ref r = lift(a, [&](const Ref &x) { return zext(x, n); }))
    return r;
  return make({Op::ZExt, a, {}, {}, n});
}
Ref Terms::sext(const Ref &a, unsigned n) {
  if (!n) return a;
  if (const APInt *ca = constant(a)) return bv(ca->sext(ca->getBitWidth() + n));
  if (Ref r = lift(a, [&](const Ref &x) { return sext(x, n); }))
    return r;
  return make({Op::SExt, a, {}, {}, n});
}

Ref Terms::eq(const Ref &a, const Ref &b) {
  if (a == b) return boolean(true);
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb) return boolean(*ca == *cb);
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return eq(x, y); }))
    return r;
  if (a.isBool()) {
    if (ca) return ca->isOne() ? b : lnot(b);
    if (cb) return cb->isOne() ? a : lnot(a);
  } else if (ca || cb) {
    // A constant against a concat compares part by part, so a pointer to a
    // known object is never equal to null or to another object's pointer.
    const Ref &c = ca ? a : b, &x = ca ? b : a;
    if (const Node *n = node(x); n && n->op == Op::Concat) {
      unsigned w = n->b.width();
      return land(eq(extract(c, c.width() - 1, w), n->a), eq(extract(c, w - 1, 0), n->b));
    }
  }
  return make({Op::Eq, a, b});
}

Ref Terms::ult(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->ult(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return ult(x, y); }))
    return r;
  if (a == b) return boolean(false);
  // Nothing is below the least value or above the greatest, here and in
  // ule, slt and sle: x >= g with g a global still zero when loaded is true.
  if ((cb && cb->isZero()) || (ca && ca->isAllOnes())) return boolean(false);
  if (int o = order(a, b, false, false); o >= 0) return boolean(o);
  return make({Op::Ult, a, b});
}
Ref Terms::ule(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->ule(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return ule(x, y); }))
    return r;
  if (a == b) return boolean(true);
  if ((ca && ca->isZero()) || (cb && cb->isAllOnes())) return boolean(true);
  if (int o = order(a, b, true, false); o >= 0) return boolean(o);
  return make({Op::Ule, a, b});
}
Ref Terms::slt(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->slt(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return slt(x, y); }))
    return r;
  if (a == b) return boolean(false);
  if ((cb && cb->isMinSignedValue()) || (ca && ca->isMaxSignedValue())) return boolean(false);
  if (int o = order(a, b, false, true); o >= 0) return boolean(o);
  return make({Op::Slt, a, b});
}
Ref Terms::sle(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->sle(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return sle(x, y); }))
    return r;
  if (a == b) return boolean(true);
  if ((ca && ca->isMinSignedValue()) || (cb && cb->isMaxSignedValue())) return boolean(true);
  if (int o = order(a, b, true, true); o >= 0) return boolean(o);
  return make({Op::Sle, a, b});
}

Ref Terms::lnot(const Ref &a) {
  if (const APInt *ca = constant(a)) return boolean(ca->isZero());
  if (const Node *n = node(a); n && n->op == Op::Not) return n->a;
  return make({Op::Not, a});
}

Ref Terms::land(const Ref &a, const Ref &b) {
  if (isFalse(a) || isFalse(b)) return boolean(false);
  if (isTrue(a)) return b;
  if (isTrue(b) || a == b) return a;
  const Node *na = node(a), *nb = node(b);
  if ((na && na->op == Op::Not && na->a == b) ||
      (nb && nb->op == Op::Not && nb->a == a))
    return boolean(false);
  // A condition the guard already has: g and c is g, and g and not c is
  // false, which prunes the edge while it is walked.
  if (int k = conjunct(a, b))
    return k > 0 ? a : boolean(false);
  return make({Op::And, a, b});
}
Ref Terms::lor(const Ref &a, const Ref &b) {
  if (isTrue(a) || isTrue(b)) return boolean(true);
  if (isFalse(a)) return b;
  if (isFalse(b) || a == b) return a;
  const Node *na = node(a), *nb = node(b);
  if ((na && na->op == Op::Not && na->a == b) ||
      (nb && nb->op == Op::Not && nb->a == a))
    return boolean(true);
  // Branches rejoining, as ESBMC's guard |= with the one-conjunct suffixes
  // it sees in practice: (p and x) or (p and y) is p and (x or y), so an
  // if/else diamond joins back to p, and p or (p and x) is p.
  bool andA = na && na->op == Op::And, andB = nb && nb->op == Op::And;
  if (andB && nb->a == a)
    return a;
  if (andA && na->a == b)
    return b;
  if (andA && andB && na->a == nb->a)
    return land(na->a, lor(na->b, nb->b));
  return make({Op::Or, a, b});
}

// Guards grow as land(parent, condition), a chain down the left side; the
// folds look that far up it, enough for nested branches and constant per
// call.
static constexpr unsigned SpineBound = 64;

int Terms::conjunct(const Ref &g, const Ref &c) const {
  auto find = [this](const void *e) -> const Node * {
    if (!e || (reinterpret_cast<uintptr_t>(e) & 1))
      return nullptr;
    auto it = nodes.find(e);
    return it == nodes.end() ? nullptr : &entries[it->second].n;
  };
  const Node *nc = find(c.id());
  const void *e = g.id();
  for (unsigned i = 0; i < SpineBound; ++i) {
    const Node *n = find(e);
    bool isAnd = n && n->op == Op::And;
    const void *lit = isAnd ? n->b.id() : e; // the bottom is a conjunct too
    if (lit == c.id())
      return 1;
    const Node *nl = find(lit);
    if ((nc && nc->op == Op::Not && nc->a.id() == lit) ||
        (nl && nl->op == Op::Not && nl->a == c))
      return -1;
    if (!isAnd)
      return 0;
    e = n->a.id();
  }
  return 0;
}

Ref Terms::lxor(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca) return ca->isOne() ? lnot(b) : b;
  if (cb) return cb->isOne() ? lnot(a) : a;
  if (a == b) return boolean(false);
  return make({Op::Xor, a, b});
}

Ref Terms::ite(const Ref &c, const Ref &a, const Ref &b) {
  if (isTrue(c) || a == b) return a;
  if (isFalse(c)) return b;
  if (a.isBool()) {
    if (isTrue(a) && isFalse(b)) return c;
    if (isFalse(a) && isTrue(b)) return lnot(c);
  }
  // An arm that is an ite sharing the other arm merges into one condition,
  // so that a choice of constants (a phi of malloc's result and null) stays
  // one ite that split sees.
  const Node *n = node(a);
  if (n && n->op == Op::Ite && n->c == b) return ite(land(c, n->a), n->b, b);
  n = node(b);
  if (n && n->op == Op::Ite && n->c == a) return ite(land(lnot(c), n->a), n->b, a);
  return make({Op::Ite, c, a, b});
}

Ref Terms::uaddo(const Ref &a, const Ref &b) {
  bool o;
  FOLD2(((void)ca->uadd_ov(*cb, o), boolean(o)))
  return make({Op::UAddO, a, b});
}
Ref Terms::saddo(const Ref &a, const Ref &b) {
  bool o;
  FOLD2(((void)ca->sadd_ov(*cb, o), boolean(o)))
  return make({Op::SAddO, a, b});
}
Ref Terms::usubo(const Ref &a, const Ref &b) {
  bool o;
  FOLD2(((void)ca->usub_ov(*cb, o), boolean(o)))
  return make({Op::USubO, a, b});
}
Ref Terms::ssubo(const Ref &a, const Ref &b) {
  bool o;
  FOLD2(((void)ca->ssub_ov(*cb, o), boolean(o)))
  return make({Op::SSubO, a, b});
}
Ref Terms::umulo(const Ref &a, const Ref &b) {
  bool o;
  FOLD2(((void)ca->umul_ov(*cb, o), boolean(o)))
  return make({Op::UMulO, a, b});
}
Ref Terms::smulo(const Ref &a, const Ref &b) {
  bool o;
  FOLD2(((void)ca->smul_ov(*cb, o), boolean(o)))
  return make({Op::SMulO, a, b});
}
#undef FOLD2

Ref Terms::select(const Ref &array, const Ref &index) {
  if (!constant(index))
    return uncached(array, index);
  auto key = std::make_pair(array.id(), index.id());
  if (auto it = selects.find(key); it != selects.end())
    return it->second;
  Ref r = uncached(array, index);
  selects.try_emplace(key, r);
  return r;
}

Ref Terms::uncached(const Ref &array, const Ref &index) {
  Ref arr = array;
  if (!lambdas.empty()) {
    for (auto l = lambdas.find(arr.id()); l != lambdas.end() && l->second.misses &&
                                           l->second.misses(index);
         l = lambdas.find(arr.id()))
      arr = l->second.below;
    if (arr != array)
      return select(arr, index);
  }
  // A read of a constant array is its value, at any index.
  unsigned steps = 256;
  if (const APInt *ci = constant(index))
    if (Ref v = read(arr, *ci, steps))
      return v;
  if (auto it = filled.find(arr.id()); it != filled.end())
    return it->second.second;
  if (!lambdas.empty() && lambdas.count(arr.id()))
    return reduce(arr, index);
  // A read of a non-nested ite whose arms include a lambda (or two constant
  // arrays) reduces each arm directly instead of constraining the lambda
  // array symbol at every read index.
  if (const Node *n = node(arr); n && n->op == Op::Ite) {
    const Node *nb = node(n->b), *nc = node(n->c);
    bool flat = (!nb || nb->op != Op::Ite) && (!nc || nc->op != Op::Ite);
    if (flat && ((!lambdas.empty() && (lambdas.count(n->b.id()) || lambdas.count(n->c.id()))) ||
                 (filled.count(n->b.id()) && filled.count(n->c.id())))) {
      Ref c = n->a, b = n->b, d = n->c;
      return ite(c, select(b, index), select(d, index));
    }
  }
  bool fresh = !memoFind({Op::Select, arr.id(), index.id(), nullptr, 0, 0});
  Ref r = make({Op::Select, arr, index});
  if (!lambdas.empty())
    if (auto u = under.find(arr.id()); fresh && u != under.end())
      for (const Ref &l : std::vector<Ref>(u->second))
        if (!memoFind({Op::Select, l.id(), index.id(), nullptr, 0, 0}))
          addConstraint(eq(make({Op::Select, l, index}), reduce(l, index)));
  return r;
}

Ref Terms::lambda(const camada::SMTSortRef &sort, const std::string &name, Def def,
                  const Ref &below, std::function<bool(const Ref &)> misses) {
  Ref r(s.mkSymbol(name, sort));
  lambdas.emplace(r.id(), Lambda{std::move(def), below, std::move(misses)});
  return r;
}

Ref Terms::reduce(const Ref &l, const Ref &index) {
  if (auto it = reduced.find({l.id(), index.id()}); it != reduced.end())
    return it->second.second;
  Ref r = lambdas.at(l.id()).def(index);
  reduced.try_emplace({l.id(), index.id()}, index, r);
  return r;
}

Ref Terms::substitute(const Ref &e, const Ref &from, const Ref &to) {
  auto &slot = freeOf[from.id()];
  if (!slot)
    slot = std::make_unique<llvm::DenseSet<const void *>>();
  llvm::DenseSet<const void *> &same = *slot;
  llvm::DenseMap<const void *, Ref> done;
  // What x becomes, or null while its operands are pending.
  auto known = [&](const Ref &x) -> Ref {
    if (x == from)
      return to;
    if (!node(x) || same.count(x.id()))
      return x;
    auto it = done.find(x.id());
    return it == done.end() ? Ref() : it->second;
  };
  // Iterative, as a guard can be deeper than the stack.
  llvm::SmallVector<Ref, 32> stack{e};
  while (!stack.empty()) {
    Ref x = stack.back();
    if (known(x)) {
      stack.pop_back();
      continue;
    }
    const Node &n = *node(x);
    Ref kids[3] = {n.a, n.b, n.c}, now[3];
    bool ready = true;
    for (unsigned i = 0; i < 3; ++i)
      if (kids[i] && !(now[i] = known(kids[i]))) {
        stack.push_back(kids[i]);
        ready = false;
      }
    if (!ready)
      continue;
    stack.pop_back();
    if (now[0] == kids[0] && now[1] == kids[1] && now[2] == kids[2])
      same.insert(x.id());
    else
      done.try_emplace(x.id(), rebuild(n, now[0], now[1], now[2]));
  }
  return known(e);
}

Ref Terms::rebuild(const Node &n, const Ref &a, const Ref &b, const Ref &c) {
  switch (n.op) {
  case Op::Add: return add(a, b);
  case Op::Sub: return sub(a, b);
  case Op::Mul: return mul(a, b);
  case Op::UDiv: return udiv(a, b);
  case Op::SDiv: return sdiv(a, b);
  case Op::URem: return urem(a, b);
  case Op::SRem: return srem(a, b);
  case Op::Shl: return shl(a, b);
  case Op::LShr: return lshr(a, b);
  case Op::AShr: return ashr(a, b);
  case Op::BVAnd: return bvand(a, b);
  case Op::BVOr: return bvor(a, b);
  case Op::BVXor: return bvxor(a, b);
  case Op::BVNot: return bvnot(a);
  case Op::Concat: return concat(a, b);
  case Op::Extract: return extract(a, n.x, n.y);
  case Op::ZExt: return zext(a, n.x);
  case Op::SExt: return sext(a, n.x);
  case Op::Eq: return eq(a, b);
  case Op::Ult: return ult(a, b);
  case Op::Ule: return ule(a, b);
  case Op::Slt: return slt(a, b);
  case Op::Sle: return sle(a, b);
  case Op::And: return land(a, b);
  case Op::Or: return lor(a, b);
  case Op::Xor: return lxor(a, b);
  case Op::Not: return lnot(a);
  case Op::Ite: return ite(a, b, c);
  case Op::UAddO: return uaddo(a, b);
  case Op::SAddO: return saddo(a, b);
  case Op::USubO: return usubo(a, b);
  case Op::SSubO: return ssubo(a, b);
  case Op::UMulO: return umulo(a, b);
  case Op::SMulO: return smulo(a, b);
  case Op::Select: return select(a, b);
  case Op::Store: return store(a, b, c);
  }
  llvm_unreachable("operation");
}

Ref Terms::read(Ref &arr, const APInt &i, unsigned &steps) {
  for (; steps > 0; --steps) {
    const Node *n = node(arr);
    if (n && n->op == Op::Ite) { // a join: both sides must hold the same value
      --steps;
      Ref a = n->b, b = n->c, x = read(a, i, steps), y = x ? read(b, i, steps) : Ref();
      return y && x == y ? x : Ref();
    }
    if (!n || n->op != Op::Store)
      break;
    const APInt *cs = constant(n->b);
    if (!cs)
      break;
    if (*cs == i)
      return n->c;
    arr = n->a;
  }
  auto it = filled.find(arr.id());
  return it != filled.end() ? it->second.second : Ref();
}

bool Terms::arithmetic(const Ref &e) {
  if (!e || e.entry())
    return false;
  // Iterative, as an ite chain can be deeper than the stack. A read from an
  // array is a leaf: what was stored there is the solver's business only
  // when the read needs it.
  auto inner = [this](const void *p) -> const Node * {
    if (!p || (reinterpret_cast<uintptr_t>(p) & 1))
      return nullptr;
    auto it = nodes.find(p);
    if (it == nodes.end())
      return nullptr;
    const Node &n = entries[it->second].n;
    return n.op == Op::Select ? nullptr : &n;
  };
  auto own = [](Op op) {
    return op == Op::Mul || op == Op::UDiv || op == Op::SDiv || op == Op::URem || op == Op::SRem;
  };
  auto known = [&](const void *p) { return !inner(p) || arith.count(p); };
  auto value = [&](const void *p) { return inner(p) && arith.lookup(p); };
  llvm::SmallVector<const void *, 32> stack{e.id()};
  while (!stack.empty()) {
    const void *p = stack.back();
    if (known(p)) {
      stack.pop_back();
      continue;
    }
    const Node &n = *inner(p);
    const void *kids[] = {n.a.id(), n.b.id(), n.c.id()};
    bool ready = true;
    if (!own(n.op))
      for (const void *k : kids)
        if (k && !known(k)) {
          stack.push_back(k);
          ready = false;
        }
    if (!ready)
      continue;
    stack.pop_back();
    bool r = own(n.op);
    for (const void *k : kids)
      r = r || (k && value(k));
    arith.try_emplace(p, r);
  }
  return value(e.id());
}

// The greatest unsigned and the least and greatest signed value of w bits,
// a w-bit value read as signed, and a signed one as its w bits.
static uint64_t umax(unsigned w) { return ~uint64_t(0) >> (64 - w); }
static int64_t smin(unsigned w) { return int64_t(~uint64_t(0) << (w - 1)); }
static int64_t smax(unsigned w) { return int64_t(umax(w) >> 1); }
static int64_t toSigned(uint64_t u, unsigned w) { return int64_t(u << (64 - w)) >> (64 - w); }
static uint64_t toUnsigned(int64_t v, unsigned w) { return uint64_t(v) & umax(w); }
// Whether lo and hi, lo <= hi, are on one side of the sign bit, where
// the unsigned and the signed order agree.
static bool oneSide(uint64_t lo, uint64_t hi, unsigned w) { return lo >> (w - 1) == hi >> (w - 1); }

std::optional<Terms::Range> Terms::range(const Ref &e, Known known, unsigned depth) {
  if (!e.isBV() || e.width() > 64)
    return std::nullopt;
  if (const APInt *c = constant(e))
    return Range{c->getZExtValue(), c->getZExtValue(), 0};
  std::optional<Range> r;
  if (auto b = bounds.find(e.id()); b != bounds.end())
    r = Range{0, b->second.second - 1, 1};
  // A deep term (a long ite chain) is left unknown.
  else if (const Node *n = node(e); n && depth <= 32) {
    auto &cache = known ? scratch : ranges;
    if (auto it = cache.find(e.id()); it != cache.end())
      r = it->second;
    else
      cache.try_emplace(e.id(), r = shape(*n, e.width(), known, depth));
  }
  std::optional<Bounds> f = known ? known(e) : std::nullopt;
  if (!f)
    return r;
  // Narrowed to [ulo, uhi], keeping its stride; empty only on a path that
  // no execution takes, which says nothing.
  using U = unsigned __int128;
  if (!r)
    return Range{f->ulo, f->uhi, f->ulo != f->uhi};
  U s = r->stride ? r->stride : 1, lo = r->lo, hi = r->hi;
  if (lo < f->ulo)
    lo += (f->ulo - lo + s - 1) / s * s;
  if (hi > f->uhi)
    hi -= (hi - f->uhi + s - 1) / s * s; // below 0, it wraps past r->hi
  if (lo > hi || hi > r->hi)
    return std::nullopt;
  return Range{uint64_t(lo), uint64_t(hi), lo == hi ? 0 : r->stride};
}

std::optional<Terms::Range> Terms::shape(const Node &n, unsigned width, Known known,
                                         unsigned depth) {
  using U = unsigned __int128;
  U max = (U(1) << width) - 1;
  // lo + stride * i up to hi, with hi moved down onto that lattice, so
  // that every value, hi among them, is lo modulo the stride.
  auto make = [&](U lo, U hi, U stride) -> std::optional<Range> {
    if (hi > max || lo > hi)
      return std::nullopt;
    hi = stride ? lo + (hi - lo) / stride * stride : lo;
    return Range{uint64_t(lo), uint64_t(hi), hi == lo ? 0 : uint64_t(stride)};
  };
  auto get = [&](const Ref &x) { return range(x, known, depth + 1); };
  // x >> by, exact when the stride is a multiple of 2^by.
  auto right = [&](const Range &v, uint64_t by) {
    uint64_t s = by >= 64 || v.stride % (uint64_t(1) << by) ? 1 : v.stride >> by;
    return by >= 64 ? make(0, 0, 0) : make(v.lo >> by, v.hi >> by, s);
  };
  std::optional<Range> r, a, b;
  switch (n.op) {
  case Op::Add:
  case Op::Sub:
  case Op::Mul:
  case Op::Concat:
    if (!(a = get(n.a)) || !(b = get(n.b)))
      break;
    if (n.op == Op::Add)
      r = make(U(a->lo) + b->lo, U(a->hi) + b->hi, std::gcd(a->stride, b->stride));
    else if (n.op == Op::Sub && a->lo >= b->hi)
      r = make(a->lo - b->hi, a->hi - b->lo, std::gcd(a->stride, b->stride));
    else if (n.op == Op::Mul && (!a->stride || !b->stride)) {
      const Range &x = a->stride ? *a : *b;
      U k = a->stride ? b->lo : a->lo;
      r = make(x.lo * k, x.hi * k, x.stride * k);
    } else if (n.op == Op::Concat) {
      unsigned low = n.b.width();
      r = make((U(a->lo) << low) + b->lo, (U(a->hi) << low) + b->hi,
               std::gcd(uint64_t(U(a->stride) << low), b->stride));
    }
    break;
  case Op::LShr:
    if (const APInt *c = constant(n.b); c && (a = get(n.a)))
      r = right(*a, c->getLimitedValue(64));
    break;
  case Op::Extract:
    if ((a = get(n.a)))
      r = right(*a, n.y);
    break;
  case Op::ZExt:
  case Op::SExt: // below the sign bit, as zext
    if ((a = get(n.a)) && (n.op == Op::ZExt || !(a->hi >> (n.a.width() - 1))))
      r = a;
    break;
  default:
    break;
  }
  return r;
}

std::optional<std::pair<int64_t, int64_t>> Terms::srange(const Ref &e, Known known,
                                                         unsigned depth) {
  if (!e.isBV() || e.width() > 64)
    return std::nullopt;
  unsigned w = e.width();
  int64_t lo = smin(w), hi = smax(w);
  auto meet = [&](int64_t l, int64_t h) {
    lo = std::max(lo, l);
    hi = std::min(hi, h);
  };
  if (std::optional<Range> r = range(e, known, depth); r && oneSide(r->lo, r->hi, w))
    meet(toSigned(r->lo, w), toSigned(r->hi, w));
  if (std::optional<Bounds> f = known ? known(e) : std::nullopt)
    meet(f->slo, f->shi);
  if (const Node *n = node(e); n && n->op == Op::SExt && depth <= 32)
    if (auto a = srange(n->a, known, depth + 1))
      meet(a->first, a->second);
  if (lo > hi)
    return std::nullopt;
  return std::make_pair(lo, hi);
}

int Terms::order(const Ref &a, const Ref &b, bool orEqual, bool sign, Known known) {
  auto settle = [orEqual](auto xlo, auto xhi, auto ylo, auto yhi) {
    if (orEqual ? xhi <= ylo : xhi < ylo)
      return 1;
    if (orEqual ? xlo > yhi : xlo >= yhi)
      return 0;
    return -1;
  };
  if (sign) {
    auto x = srange(a, known, 0), y = x ? srange(b, known, 0) : std::nullopt;
    return x && y ? settle(x->first, x->second, y->first, y->second) : -1;
  }
  std::optional<Range> x = range(a, known, 0), y = x ? range(b, known, 0) : std::nullopt;
  return x && y ? settle(x->lo, x->hi, y->lo, y->hi) : -1;
}

int Terms::decide(const Ref &cmp, Known known) {
  const Node *n = node(cmp);
  if (!n)
    return -1;
  scratch.clear();
  switch (n->op) {
  case Op::Not: {
    int d = decide(n->a, known);
    return d < 0 ? d : !d;
  }
  case Op::Ult:
  case Op::Ule:
  case Op::Slt:
  case Op::Sle:
    return order(n->a, n->b, n->op == Op::Ule || n->op == Op::Sle,
                 n->op == Op::Slt || n->op == Op::Sle, known);
  case Op::Eq: { // unequal when the ranges, or the signed ones, are apart
    std::optional<Range> x = range(n->a, known, 0), y = range(n->b, known, 0);
    if (x && y && (x->hi < y->lo || y->hi < x->lo))
      return 0;
    auto sx = srange(n->a, known, 0), sy = srange(n->b, known, 0);
    if (sx && sy && (sx->second < sy->first || sy->second < sx->first))
      return 0;
    return -1;
  }
  default:
    return -1;
  }
}

void Terms::narrow(const Ref &cond, bool holds, Known known,
                   llvm::function_ref<void(const Ref &, const Bounds &)> set) {
  unsigned budget = 64;
  literals(cond, holds, [&](const Node &n, bool h) {
    bool sign = n.op == Op::Slt || n.op == Op::Sle;
    if ((!sign && n.op != Op::Ult && n.op != Op::Ule) || n.a.width() > 64)
      return;
    // x < y, or x <= y; when the literal fails, its sides swap: !(a < b) is
    // b <= a.
    bool strict = (n.op == Op::Ult || n.op == Op::Slt) == h;
    const Ref &x = h ? n.a : n.b, &y = h ? n.b : n.a;
    unsigned w = x.width();
    scratch.clear();
    auto now = [&](const Ref &e) {
      Bounds b{0, umax(w), smin(w), smax(w)};
      if (std::optional<Range> r = range(e, known, 0))
        b.ulo = r->lo, b.uhi = r->hi;
      if (auto s = srange(e, known, 0))
        b.slo = s->first, b.shi = s->second;
      return b;
    };
    Bounds bx = now(x), by = now(y), nx = bx, ny = by;
    // x <= max(y) - strict and y >= min(x) + strict; past the type's
    // limits, no execution takes the path.
    if (sign) {
      if (strict && (by.shi == smin(w) || bx.slo == smax(w)))
        return;
      nx.shi = std::min(bx.shi, by.shi - strict);
      ny.slo = std::max(by.slo, bx.slo + strict);
    } else {
      if (strict && (by.uhi == 0 || bx.ulo == umax(w)))
        return;
      nx.uhi = std::min(bx.uhi, by.uhi - strict);
      ny.ulo = std::max(by.ulo, bx.ulo + strict);
    }
    auto update = [&](const Ref &e, const Bounds &was, Bounds b) {
      // Each order bounds the other on one side of the sign bit; twice
      // carries a bound across both ways.
      for (int i = 0; i < 2; ++i) {
        if (b.ulo > b.uhi || b.slo > b.shi)
          return;
        if (oneSide(b.ulo, b.uhi, w))
          b.slo = std::max(b.slo, toSigned(b.ulo, w)), b.shi = std::min(b.shi, toSigned(b.uhi, w));
        if ((b.slo < 0) == (b.shi < 0))
          b.ulo = std::max(b.ulo, toUnsigned(b.slo, w)), b.uhi = std::min(b.uhi, toUnsigned(b.shi, w));
      }
      if (b.ulo <= b.uhi && b.slo <= b.shi && !(b == was) && !constant(e))
        set(e, b);
    };
    update(x, bx, nx);
    update(y, by, ny);
  }, budget);
}

unsigned Terms::zeros(const Ref &e, unsigned depth) {
  if (const APInt *c = constant(e))
    return c->countr_zero();
  const Node *n = node(e);
  if (!n || depth > 32 || (n->op != Op::Add && n->op != Op::Sub && n->op != Op::Mul))
    return 0;
  unsigned x = zeros(n->a, depth + 1), y = zeros(n->b, depth + 1);
  return n->op == Op::Mul ? std::min(x + y, e.width()) : std::min(x, y);
}

Ref Terms::store(const Ref &array, const Ref &index, const Ref &value) {
  // Storing a constant array's own value changes nothing. A flush stores each
  // byte's provenance, which for a zero-filled object's own arrays is the
  // value they already hold, so that array stays constant.
  if (auto it = filled.find(array.id()); it != filled.end() && it->second.second == value)
    return array;
  return make({Op::Store, array, index, value});
}

Ref Terms::constArray(const camada::SMTSortRef &index, const Ref &value) {
  Ref r(s.mkArrayConst(index, solverTerm(value)));
  filled.try_emplace(r.id(), r, value);
  return r;
}

} // namespace vereda::encode
