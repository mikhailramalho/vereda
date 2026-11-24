// Stage encode: the term builder every stage uses in place of camada's
// constructors. It folds as terms are built (plan, section 4.2): constants,
// identities, an ite with a constant guard, extract over concat, a
// comparison against the least or greatest value, and a select at a
// constant index over stores at other constant indices and joins. It also
// shares terms: building the same term twice returns the same reference,
// so pointer equality is structural equality for terms built here.
//
// A bit-vector constant is a term of the solver's arena that only Terms can
// read: its value, and the solver's own term for it, made the first time the
// constant goes into a term the solver sees. Most constants fold away
// without ever getting one. camada's backends cast every term they are
// given to their own type unchecked, so such a constant must not reach
// camada: Terms swaps in the solver's term for the operations it builds,
// and other code that hands camada a bit-vector term that may be one passes
// it through solverTerm. Bools, floats and arrays are never such a constant.
#pragma once
#include <camada/camada.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace vereda::encode {

class Terms {
public:
  using Ref = camada::SMTExprRef;
  explicit Terms(camada::SMTSolver &s);
  camada::SMTSolver &solver() { return s; }

  // The value of a constant term (width 1 for Bool), or null.
  const llvm::APInt *constant(const Ref &e) const;
  bool isTrue(const Ref &e) const { return e.get() == bools[1].get(); }
  bool isFalse(const Ref &e) const { return e.get() == bools[0].get(); }
  // e as camada can take it: the solver's term for a bit-vector constant,
  // made here the first time, and any other term as it is.
  Ref solverTerm(const Ref &e);
  // A term camada gave back, with the solver's term for a constant
  // replaced by the constant: mkIEEEFPToBV gives back the bits a float
  // was made from.
  Ref fromSolver(const Ref &e) const;

  Ref bv(const llvm::APInt &v);
  // v must fit in width bits, signed or unsigned: with assertions off APInt
  // keeps the bits that do not, and every later fold goes wrong (#59).
  Ref bv(int64_t v, unsigned width) {
    if (!llvm::isIntN(width, v) && !llvm::isUIntN(width, static_cast<uint64_t>(v)))
      llvm::report_fatal_error(llvm::Twine("constant ") + llvm::Twine(v) + " does not fit in " +
                               llvm::Twine(width) + " bits");
    return bv(llvm::APInt(width, v, /*isSigned=*/v < 0));
  }
  Ref boolean(bool b);
  Ref symbol(const std::string &name, const camada::SMTSortRef &sort) {
    return s.mkSymbol(name, sort);
  }

  Ref add(const Ref &a, const Ref &b);
  Ref sub(const Ref &a, const Ref &b);
  Ref mul(const Ref &a, const Ref &b);
  Ref udiv(const Ref &a, const Ref &b);
  Ref sdiv(const Ref &a, const Ref &b);
  Ref urem(const Ref &a, const Ref &b);
  Ref srem(const Ref &a, const Ref &b);
  Ref shl(const Ref &a, const Ref &b);
  Ref lshr(const Ref &a, const Ref &b);
  Ref ashr(const Ref &a, const Ref &b);
  Ref bvand(const Ref &a, const Ref &b);
  Ref bvor(const Ref &a, const Ref &b);
  Ref bvxor(const Ref &a, const Ref &b);
  Ref bvnot(const Ref &a);
  Ref concat(const Ref &hi, const Ref &lo);
  Ref extract(const Ref &a, unsigned hi, unsigned lo);
  Ref zext(const Ref &a, unsigned n);
  Ref sext(const Ref &a, unsigned n);

  Ref eq(const Ref &a, const Ref &b);
  Ref ult(const Ref &a, const Ref &b);
  Ref ule(const Ref &a, const Ref &b);
  Ref ugt(const Ref &a, const Ref &b) { return ult(b, a); }
  Ref uge(const Ref &a, const Ref &b) { return ule(b, a); }
  Ref slt(const Ref &a, const Ref &b);
  Ref sle(const Ref &a, const Ref &b);
  Ref sgt(const Ref &a, const Ref &b) { return slt(b, a); }
  Ref sge(const Ref &a, const Ref &b) { return sle(b, a); }

  Ref land(const Ref &a, const Ref &b);
  Ref lor(const Ref &a, const Ref &b);
  Ref lxor(const Ref &a, const Ref &b);
  Ref lnot(const Ref &a);
  Ref implies(const Ref &a, const Ref &b) { return lor(lnot(a), b); }
  Ref ite(const Ref &c, const Ref &a, const Ref &b);

  // Whether the operation overflows.
  Ref uaddo(const Ref &a, const Ref &b);
  Ref saddo(const Ref &a, const Ref &b);
  Ref usubo(const Ref &a, const Ref &b);
  Ref ssubo(const Ref &a, const Ref &b);
  Ref umulo(const Ref &a, const Ref &b);
  Ref smulo(const Ref &a, const Ref &b);

  Ref select(const Ref &array, const Ref &index);
  Ref store(const Ref &array, const Ref &index, const Ref &value);
  // An array holding value at every index of the sort; a read of it folds.
  Ref constArray(const camada::SMTSortRef &index, const Ref &value);
  // A fresh array named name whose element at i is def(i), which camada has
  // no term for. A read of it reduces to def(i) as it is built, as ESBMC's
  // simplifier reduces its lambda. A read through stores or choices above it
  // stays a select, and the definition at that index becomes a constraint:
  // the solver sees the array only at the indices it is read at.
  Ref lambda(const camada::SMTSortRef &sort, const std::string &name,
             std::function<Ref(const Ref &)> def);
  // e with the symbol from replaced by to, folded again.
  Ref substitute(const Ref &e, const Ref &from, const Ref &to);

  // Whether guard entails lit by its and/or structure alone: lit is a
  // conjunct of guard, or implied by both sides of a disjunction.
  bool entails(const Ref &guard, const Ref &lit);
  // Calls f(x, c) for each term x that cond being true, or false when
  // !holds, makes equal to a bitvector constant c: an x = c found through
  // the conjuncts of cond, the disjuncts of its negation, and negations.
  // Only Terms' own = counts, which is equality of the terms: never a
  // float's fp.eq (+0 equals -0) nor anything opaque.
  void equalities(const Ref &cond, bool holds,
                  llvm::function_ref<void(const Ref &, const Ref &)> f) const;
  // e as ite(c, x, y) with constant x and y.
  bool split(const Ref &e, Ref &c, Ref &x, Ref &y) const;
  // e as ite(c, x, y), whatever x and y are.
  bool choice(const Ref &e, Ref &c, Ref &x, Ref &y) const;
  // e as store(a, i, v).
  bool stored(const Ref &e, Ref &a, Ref &i, Ref &v) const;
  // The value every index of e holds, if e is a constant array; else null.
  Ref filledWith(const Ref &e) const;
  // Whether e multiplies, divides or takes a remainder, outside of arrays.
  bool arithmetic(const Ref &e);

private:
  // equalities over at most budget nodes of cond.
  void equalities(const Ref &cond, bool holds,
                  llvm::function_ref<void(const Ref &, const Ref &)> f, unsigned &budget) const;
  // The value at constant index i of arr, past stores at other constant
  // indices, which arr moves over, and through joins whose two sides hold
  // the same value there (as a stream pointer does after a branch that
  // stored elsewhere). Null when it is not found within steps nodes, a cap
  // so that a read past many stores stays a select.
  Ref read(Ref &arr, const llvm::APInt &i, unsigned &steps);
  // select without the cache below.
  Ref uncached(const Ref &array, const Ref &index);
  enum class Op : uint8_t {
    Add, Sub, Mul, UDiv, SDiv, URem, SRem, Shl, LShr, AShr, BVAnd, BVOr, BVXor,
    BVNot, Concat, Extract, ZExt, SExt, Eq, Ult, Ule, Slt, Sle, And, Or, Xor,
    Not, Ite, UAddO, SAddO, USubO, SSubO, UMulO, SMulO, Select, Store,
  };
  struct Node {
    Op op;
    Ref a, b, c;
    unsigned x = 0, y = 0; // extract bounds, extension width
  };
  struct Key {
    Op op;
    const camada::SMTExpr *a, *b, *c;
    unsigned x, y;
    bool operator==(const Key &o) const {
      return op == o.op && a == o.a && b == o.b && c == o.c && x == o.x && y == o.y;
    }
  };
  struct KeyHash {
    size_t operator()(const Key &k) const;
  };

  const Node *node(const Ref &e) const;
  Ref make(Node n);
  // Whether c (1) or its negation (-1) is a conjunct of g, found going up
  // g's left spine of ands for a bounded number of steps; 0 when neither is.
  int conjunct(const Ref &g, const Ref &c) const;
  // f pushed through an ite with constant branches, so that it folds on
  // each; null when no operand is one, or two have different conditions.
  // Pointers from an allocation that may fail, ite(ok, p, null), stay
  // constant on each side through offsets, comparisons and memory.
  Ref lift(const Ref &a, const std::function<Ref(const Ref &)> &f);
  Ref lift(const Ref &a, const Ref &b, const std::function<Ref(const Ref &, const Ref &)> &f);
  Ref build(const Node &n);
  // n's operation applied to other operands.
  Ref rebuild(const Node &n, const Ref &a, const Ref &b, const Ref &c);
  // def(index) of the lambda l, once per index.
  Ref reduce(const Ref &l, const Ref &index);

  camada::SMTSolver &s;
  std::unordered_map<Key, Ref, KeyHash> memo;
  // Node-based maps: folding recurses while holding references into them.
  std::unordered_map<const camada::SMTExpr *, Node> nodes;
  llvm::DenseMap<llvm::APInt, Ref> bvs;
  // Per solver term made for a constant, the constant.
  llvm::DenseMap<const camada::SMTExpr *, Ref> made;
  Ref bools[2];
  unsigned joinDepth = 0; // nesting of the concat join, bounded
  struct PairHash {
    size_t operator()(const std::pair<const camada::SMTExpr *, const camada::SMTExpr *> &p) const {
      return std::hash<const void *>()(p.first) * 31 + std::hash<const void *>()(p.second);
    }
  };
  std::unordered_map<std::pair<const camada::SMTExpr *, const camada::SMTExpr *>, bool, PairHash>
      implied;
  bool entails(const Ref &guard, const Ref &lit, unsigned depth);
  std::unordered_map<const camada::SMTExpr *, bool> arith;
  // Constant arrays built here (kept alive, so the key stays theirs) and
  // the value each holds.
  std::unordered_map<const camada::SMTExpr *, std::pair<Ref, Ref>> filled;
  std::unordered_map<const camada::SMTExpr *, std::function<Ref(const Ref &)>> lambdas;
  // Per store or choice of arrays, the lambdas it reads through without
  // passing another one.
  std::unordered_map<const camada::SMTExpr *, std::vector<Ref>> under;
  // Per lambda and index, the index (kept alive) and the definition there.
  std::unordered_map<std::pair<const camada::SMTExpr *, const camada::SMTExpr *>,
                     std::pair<Ref, Ref>, PairHash>
      reduced;
  // Per array and constant index, what select gave: a read walks up to 256
  // stores, and a load of an element reads it once per byte.
  std::unordered_map<std::pair<const camada::SMTExpr *, const camada::SMTExpr *>, Ref, PairHash>
      selects;
  // Per symbol, the terms found not to contain it.
  std::unordered_map<const camada::SMTExpr *, std::unordered_set<const camada::SMTExpr *>> freeOf;
};

} // namespace vereda::encode
