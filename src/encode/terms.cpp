#include "encode/terms.h"

#include <algorithm>
#include <llvm/ADT/Hashing.h>

using llvm::APInt;

namespace vereda::encode {

using Ref = Terms::Ref;

size_t Terms::KeyHash::operator()(const Key &k) const {
  return llvm::hash_combine(static_cast<uint8_t>(k.op), k.a, k.b, k.c, k.x, k.y);
}

const APInt *Terms::constant(const Ref &e) const {
  auto it = values.find(e.get());
  return it == values.end() ? nullptr : &it->second;
}
bool Terms::isTrue(const Ref &e) const {
  const APInt *v = constant(e);
  return v && e->isBoolSort() && v->isOne();
}
bool Terms::isFalse(const Ref &e) const {
  const APInt *v = constant(e);
  return v && e->isBoolSort() && v->isZero();
}

const Terms::Node *Terms::node(const Ref &e) const {
  auto it = nodes.find(e.get());
  return it == nodes.end() ? nullptr : &it->second;
}

Ref Terms::bv(const APInt &v) {
  Ref &r = bvs[v];
  if (!r) {
    // Up to 64 bits, camada hands the value to the solver as an integer;
    // a string of digits would be parsed back, twice in Bitwuzla.
    unsigned w = v.getBitWidth();
    if (w <= 64) {
      r = s.mkBVFromDec(static_cast<int64_t>(v.getZExtValue()), w);
    } else {
      std::string bits(w, '0');
      for (unsigned i = 0; i < w; ++i)
        if (v[i])
          bits[w - 1 - i] = '1';
      r = s.mkBVFromBin(bits, w);
    }
    values.try_emplace(r.get(), v);
  }
  return r;
}

Ref Terms::boolean(bool b) {
  Ref &r = bools[b];
  if (!r) {
    r = s.mkBool(b);
    values.try_emplace(r.get(), APInt(1, b));
  }
  return r;
}

bool Terms::entails(const Ref &guard, const Ref &lit) { return entails(guard, lit, 0); }

bool Terms::entails(const Ref &guard, const Ref &lit, unsigned depth) {
  if (guard.get() == lit.get() || isTrue(lit) || isFalse(guard))
    return true;
  const Node *n = node(guard);
  // Deep guards give up, which only loses a fold.
  if (!n || (n->op != Op::And && n->op != Op::Or) || depth > 1000)
    return false;
  auto [it, fresh] = implied.try_emplace({guard.get(), lit.get()}, false);
  if (!fresh)
    return it->second;
  Ref a = n->a, b = n->b;
  bool r = n->op == Op::And ? entails(a, lit, depth + 1) || entails(b, lit, depth + 1)
                            : entails(a, lit, depth + 1) && entails(b, lit, depth + 1);
  implied[{guard.get(), lit.get()}] = r; // the map may have rehashed
  return r;
}

void Terms::equalities(const Ref &cond, bool holds,
                       llvm::function_ref<void(const Ref &, const Ref &)> f) const {
  // Conditions share subterms, so a walk without a bound can take
  // exponential time; equalities sit near the top.
  unsigned budget = 64;
  equalities(cond, holds, f, budget);
}

void Terms::equalities(const Ref &cond, bool holds,
                       llvm::function_ref<void(const Ref &, const Ref &)> f,
                       unsigned &budget) const {
  const Node *n = node(cond);
  if (!n || !budget)
    return;
  --budget;
  switch (n->op) {
  case Op::Not: equalities(n->a, !holds, f, budget); return;
  case Op::And:
  case Op::Or:
    if ((n->op == Op::And) == holds) { // both sides hold, or both fail
      equalities(n->a, holds, f, budget);
      equalities(n->b, holds, f, budget);
    }
    return;
  case Op::Ite: // a Bool ite with a constant arm: a && b, a || b
    if (constant(n->b) && constant(n->b)->isZero() == holds) {
      equalities(n->a, false, f, budget);
      equalities(n->c, holds, f, budget);
    } else if (constant(n->c) && constant(n->c)->isZero() == holds) {
      equalities(n->a, true, f, budget);
      equalities(n->b, holds, f, budget);
    }
    return;
  case Op::Eq:
    if (!holds || n->a->isBoolSort())
      return;
    if (constant(n->b) && !constant(n->a))
      f(n->a, n->b);
    else if (constant(n->a) && !constant(n->b))
      f(n->b, n->a);
    return;
  default: return;
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
  auto it = filled.find(e.get());
  return it == filled.end() ? Ref() : it->second.second;
}

Ref Terms::lift(const Ref &a, const std::function<Ref(const Ref &)> &f) {
  Ref c, x, y;
  return split(a, c, x, y) ? ite(c, f(x), f(y)) : Ref();
}

Ref Terms::lift(const Ref &a, const Ref &b, const std::function<Ref(const Ref &, const Ref &)> &f) {
  Ref ca, cb, ax = a, ay = a, bx = b, by = b;
  bool sa = split(a, ca, ax, ay), sb = split(b, cb, bx, by);
  if ((!sa && !sb) || (sa && sb && ca.get() != cb.get()) || (!sa && !constant(a)) ||
      (!sb && !constant(b)))
    return {};
  return ite(sa ? ca : cb, f(ax, bx), f(ay, by));
}

Ref Terms::make(Node n) {
  Key k{n.op, n.a.get(), n.b ? n.b.get() : nullptr, n.c ? n.c.get() : nullptr, n.x, n.y};
  auto [it, fresh] = memo.try_emplace(k);
  if (fresh) {
    it->second = build(n);
    std::vector<Ref> reads;
    auto add = [&](const Ref &l) {
      if (std::none_of(reads.begin(), reads.end(), [&](const Ref &r) { return r.get() == l.get(); }))
        reads.push_back(l);
    };
    auto through = [&](const Ref &array) {
      if (lambdas.count(array.get()))
        add(array);
      else if (auto u = under.find(array.get()); u != under.end())
        for (const Ref &l : u->second)
          add(l);
    };
    if (n.op == Op::Store)
      through(n.a);
    else if (n.op == Op::Ite)
      through(n.b), through(n.c);
    if (!reads.empty())
      under.emplace(it->second.get(), std::move(reads));
    nodes.try_emplace(it->second.get(), std::move(n));
  }
  return it->second;
}

Ref Terms::build(const Node &n) {
  const Ref &a = n.a, &b = n.b;
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
  case Op::Ite: return s.mkIte(a, b, n.c);
  case Op::UAddO: return s.mkBVUAddOverflow(a, b);
  case Op::SAddO: return s.mkBVSAddOverflow(a, b);
  case Op::USubO: return s.mkBVUSubOverflow(a, b);
  case Op::SSubO: return s.mkBVSSubOverflow(a, b);
  case Op::UMulO: return s.mkBVUMulOverflow(a, b);
  case Op::SMulO: return s.mkBVSMulOverflow(a, b);
  case Op::Select: return s.mkArraySelect(a, b);
  case Op::Store: return s.mkArrayStore(a, b, n.c);
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
  if (a.get() == b.get()) return bv(0, a->getWidth());
  if (const Node *n = node(a); n && n->op == Op::Add) { // (x + y) - x
    if (n->a.get() == b.get()) return n->b;
    if (n->b.get() == b.get()) return n->a;
  }
  return make({Op::Sub, a, b});
}

Ref Terms::mul(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca * *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return mul(x, y); }))
    return r;
  if ((ca && ca->isZero()) || (cb && cb->isZero())) return bv(0, a->getWidth());
  if (cb && cb->isOne()) return a;
  if (ca && ca->isOne()) return b;
  return make({Op::Mul, a, b});
}

// Division by zero is left to the solver, whose semantics differ from APInt's.
Ref Terms::udiv(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb && !cb->isZero()) return bv(ca->udiv(*cb));
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
  if ((ca && ca->isZero()) || (cb && cb->isZero())) return bv(0, a->getWidth());
  if (cb && cb->isAllOnes()) return a;
  if (ca && ca->isAllOnes()) return b;
  if (a.get() == b.get()) return a;
  return make({Op::BVAnd, a, b});
}
Ref Terms::bvor(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca | *cb))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return bvor(x, y); }))
    return r;
  if (cb && cb->isZero()) return a;
  if (ca && ca->isZero()) return b;
  if (a.get() == b.get()) return a;
  return make({Op::BVOr, a, b});
}
Ref Terms::bvxor(const Ref &a, const Ref &b) {
  FOLD2(bv(*ca ^ *cb))
  if (cb && cb->isZero()) return a;
  if (ca && ca->isZero()) return b;
  if (a.get() == b.get()) return bv(0, a->getWidth());
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
  if (nh && nl && nh->op == Op::Ite && nl->op == Op::Ite && nh->a.get() == nl->a.get() &&
      joinDepth < 1000) {
    Key k{Op::Concat, hi.get(), lo.get(), nullptr, 0, 0};
    if (auto it = memo.find(k); it != memo.end())
      return it->second;
    Ref c = nh->a, ha = nh->b, hb = nh->c, la = nl->b, lb = nl->c;
    ++joinDepth;
    Ref r = ite(c, concat(ha, la), concat(hb, lb));
    --joinDepth;
    memo.emplace(k, r);
    return r;
  }
  // x[h:m+1] ++ x[m:l] is x[h:l]: bytes read back from one stored value.
  if (nh && nl && nh->op == Op::Extract && nl->op == Op::Extract &&
      nh->a.get() == nl->a.get() && nh->y == nl->x + 1)
    return extract(nh->a, nh->x, nl->y);
  return make({Op::Concat, hi, lo});
}

Ref Terms::extract(const Ref &a, unsigned hi, unsigned lo) {
  if (lo == 0 && hi + 1 == a->getWidth()) return a;
  if (const APInt *ca = constant(a)) return bv(ca->extractBits(hi - lo + 1, lo));
  if (Ref r = lift(a, [&](const Ref &x) { return extract(x, hi, lo); }))
    return r;
  if (const Node *n = node(a)) {
    switch (n->op) {
    case Op::Extract:
      return extract(n->a, hi + n->y, lo + n->y);
    case Op::Concat: {
      unsigned w = n->b->getWidth();
      if (hi < w) return extract(n->b, hi, lo);
      if (lo >= w) return extract(n->a, hi - w, lo - w);
      break;
    }
    case Op::ZExt:
    case Op::SExt:
      if (hi < n->a->getWidth()) return extract(n->a, hi, lo);
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
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return eq(x, y); }))
    return r;
  if (a.get() == b.get()) return boolean(true);
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca && cb) return boolean(*ca == *cb);
  if (a->isBoolSort()) {
    if (ca) return ca->isOne() ? b : lnot(b);
    if (cb) return cb->isOne() ? a : lnot(a);
  } else if (ca || cb) {
    // A constant against a concat compares part by part, so a pointer to a
    // known object is never equal to null or to another object's pointer.
    const Ref &c = ca ? a : b, &x = ca ? b : a;
    if (const Node *n = node(x); n && n->op == Op::Concat) {
      unsigned w = n->b->getWidth();
      return land(eq(extract(c, c->getWidth() - 1, w), n->a), eq(extract(c, w - 1, 0), n->b));
    }
  }
  return make({Op::Eq, a, b});
}

Ref Terms::ult(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->ult(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return ult(x, y); }))
    return r;
  if (a.get() == b.get()) return boolean(false);
  // Nothing is below the least value or above the greatest, here and in
  // ule, slt and sle: x >= g with g a global still zero when loaded is true.
  if ((cb && cb->isZero()) || (ca && ca->isAllOnes())) return boolean(false);
  return make({Op::Ult, a, b});
}
Ref Terms::ule(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->ule(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return ule(x, y); }))
    return r;
  if (a.get() == b.get()) return boolean(true);
  if ((ca && ca->isZero()) || (cb && cb->isAllOnes())) return boolean(true);
  return make({Op::Ule, a, b});
}
Ref Terms::slt(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->slt(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return slt(x, y); }))
    return r;
  if (a.get() == b.get()) return boolean(false);
  if ((cb && cb->isMinSignedValue()) || (ca && ca->isMaxSignedValue())) return boolean(false);
  return make({Op::Slt, a, b});
}
Ref Terms::sle(const Ref &a, const Ref &b) {
  FOLD2(boolean(ca->sle(*cb)))
  if (Ref r = lift(a, b, [this](const Ref &x, const Ref &y) { return sle(x, y); }))
    return r;
  if (a.get() == b.get()) return boolean(true);
  if ((ca && ca->isMinSignedValue()) || (cb && cb->isMaxSignedValue())) return boolean(true);
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
  if (isTrue(b) || a.get() == b.get()) return a;
  const Node *na = node(a), *nb = node(b);
  if ((na && na->op == Op::Not && na->a.get() == b.get()) ||
      (nb && nb->op == Op::Not && nb->a.get() == a.get()))
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
  if (isFalse(b) || a.get() == b.get()) return a;
  const Node *na = node(a), *nb = node(b);
  if ((na && na->op == Op::Not && na->a.get() == b.get()) ||
      (nb && nb->op == Op::Not && nb->a.get() == a.get()))
    return boolean(true);
  // Branches rejoining, as ESBMC's guard |= with the one-conjunct suffixes
  // it sees in practice: (p and x) or (p and y) is p and (x or y), so an
  // if/else diamond joins back to p, and p or (p and x) is p.
  bool andA = na && na->op == Op::And, andB = nb && nb->op == Op::And;
  if (andB && nb->a.get() == a.get())
    return a;
  if (andA && na->a.get() == b.get())
    return b;
  if (andA && andB && na->a.get() == nb->a.get())
    return land(na->a, lor(na->b, nb->b));
  return make({Op::Or, a, b});
}

// Guards grow as land(parent, condition), a chain down the left side; the
// folds look that far up it, enough for nested branches and constant per
// call.
static constexpr unsigned SpineBound = 64;

int Terms::conjunct(const Ref &g, const Ref &c) const {
  auto find = [this](const camada::SMTExpr *e) {
    auto it = nodes.find(e);
    return it == nodes.end() ? nullptr : &it->second;
  };
  const Node *nc = find(c.get());
  const camada::SMTExpr *e = g.get();
  for (unsigned i = 0; i < SpineBound; ++i) {
    const Node *n = find(e);
    bool isAnd = n && n->op == Op::And;
    const camada::SMTExpr *lit = isAnd ? n->b.get() : e; // the bottom is a conjunct too
    if (lit == c.get())
      return 1;
    const Node *nl = find(lit);
    if ((nc && nc->op == Op::Not && nc->a.get() == lit) ||
        (nl && nl->op == Op::Not && nl->a.get() == c.get()))
      return -1;
    if (!isAnd)
      return 0;
    e = n->a.get();
  }
  return 0;
}

Ref Terms::lxor(const Ref &a, const Ref &b) {
  const APInt *ca = constant(a), *cb = constant(b);
  if (ca) return ca->isOne() ? lnot(b) : b;
  if (cb) return cb->isOne() ? lnot(a) : a;
  if (a.get() == b.get()) return boolean(false);
  return make({Op::Xor, a, b});
}

Ref Terms::ite(const Ref &c, const Ref &a, const Ref &b) {
  if (isTrue(c) || a.get() == b.get()) return a;
  if (isFalse(c)) return b;
  if (a->isBoolSort()) {
    if (isTrue(a) && isFalse(b)) return c;
    if (isFalse(a) && isTrue(b)) return lnot(c);
  }
  // An arm that is an ite sharing the other arm merges into one condition,
  // so that a choice of constants (a phi of malloc's result and null) stays
  // one ite that split sees.
  const Node *n = node(a);
  if (n && n->op == Op::Ite && n->c.get() == b.get()) return ite(land(c, n->a), n->b, b);
  n = node(b);
  if (n && n->op == Op::Ite && n->c.get() == a.get()) return ite(land(lnot(c), n->a), n->b, a);
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
  // A read of a constant array is its value, at any index.
  Ref arr = array;
  unsigned steps = 256;
  if (const APInt *ci = constant(index))
    if (Ref v = read(arr, *ci, steps))
      return v;
  if (auto it = filled.find(arr.get()); it != filled.end())
    return it->second.second;
  if (lambdas.count(arr.get()))
    return reduce(arr, index);
  bool fresh = !memo.count({Op::Select, arr.get(), index.get(), nullptr, 0, 0});
  Ref r = make({Op::Select, arr, index});
  if (auto u = under.find(arr.get()); fresh && u != under.end())
    for (const Ref &l : std::vector<Ref>(u->second))
      if (!memo.count({Op::Select, l.get(), index.get(), nullptr, 0, 0}))
        s.addConstraint(eq(make({Op::Select, l, index}), reduce(l, index)));
  return r;
}

Ref Terms::lambda(const camada::SMTSortRef &sort, const std::string &name,
                  std::function<Ref(const Ref &)> def) {
  Ref r = s.mkSymbol(name, sort);
  lambdas.emplace(r.get(), std::move(def));
  return r;
}

Ref Terms::reduce(const Ref &l, const Ref &index) {
  if (auto it = reduced.find({l.get(), index.get()}); it != reduced.end())
    return it->second.second;
  Ref r = lambdas.at(l.get())(index);
  reduced.try_emplace({l.get(), index.get()}, index, r);
  return r;
}

Ref Terms::substitute(const Ref &e, const Ref &from, const Ref &to) {
  std::unordered_set<const camada::SMTExpr *> &same = freeOf[from.get()];
  std::unordered_map<const camada::SMTExpr *, Ref> done;
  // What x becomes, or null while its operands are pending.
  auto known = [&](const Ref &x) -> Ref {
    if (x.get() == from.get())
      return to;
    if (!node(x) || same.count(x.get()))
      return x;
    auto it = done.find(x.get());
    return it == done.end() ? Ref() : it->second;
  };
  // Iterative, as a guard can be deeper than the stack.
  std::vector<Ref> stack{e};
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
    auto raw = [](const Ref &r) { return r ? r.get() : nullptr; };
    if (raw(now[0]) == raw(kids[0]) && raw(now[1]) == raw(kids[1]) && raw(now[2]) == raw(kids[2]))
      same.insert(x.get());
    else
      done[x.get()] = rebuild(n, now[0], now[1], now[2]);
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
      return y && x.get() == y.get() ? x : Ref();
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
  auto it = filled.find(arr.get());
  return it != filled.end() ? it->second.second : Ref();
}

bool Terms::arithmetic(const Ref &e) {
  // Iterative, as an ite chain can be deeper than the stack. A read from an
  // array is a leaf: what was stored there is the solver's business only
  // when the read needs it.
  auto inner = [this](const camada::SMTExpr *p) -> const Node * {
    auto it = nodes.find(p);
    return it == nodes.end() || it->second.op == Op::Select ? nullptr : &it->second;
  };
  auto own = [](Op op) {
    return op == Op::Mul || op == Op::UDiv || op == Op::SDiv || op == Op::URem || op == Op::SRem;
  };
  auto known = [&](const camada::SMTExpr *p) { return !inner(p) || arith.count(p); };
  auto value = [&](const camada::SMTExpr *p) { return inner(p) && arith.at(p); };
  std::vector<const camada::SMTExpr *> stack{e.get()};
  while (!stack.empty()) {
    const camada::SMTExpr *p = stack.back();
    if (known(p)) {
      stack.pop_back();
      continue;
    }
    const Node &n = *inner(p);
    auto raw = [](const Ref &r) { return r ? r.get() : nullptr; };
    const camada::SMTExpr *kids[] = {raw(n.a), raw(n.b), raw(n.c)};
    bool ready = true;
    if (!own(n.op))
      for (const camada::SMTExpr *k : kids)
        if (k && !known(k)) {
          stack.push_back(k);
          ready = false;
        }
    if (!ready)
      continue;
    stack.pop_back();
    bool r = own(n.op);
    for (const camada::SMTExpr *k : kids)
      r = r || (k && value(k));
    arith.emplace(p, r);
  }
  return value(e.get());
}

Ref Terms::store(const Ref &array, const Ref &index, const Ref &value) {
  // Storing a constant array's own value changes nothing. A flush stores each
  // byte's provenance, which for a zero-filled object's own arrays is the
  // value they already hold, so that array stays constant.
  if (auto it = filled.find(array.get()); it != filled.end() && it->second.second.get() == value.get())
    return array;
  return make({Op::Store, array, index, value});
}

Ref Terms::constArray(const camada::SMTSortRef &index, const Ref &value) {
  Ref r = s.mkArrayConst(index, value);
  filled.emplace(r.get(), std::make_pair(r, value));
  return r;
}

} // namespace vereda::encode
