// Stage encode: the term builder every stage uses in place of camada's
// constructors. It folds as terms are built (plan, section 4.2): constants,
// identities, an ite with a constant guard, extract over concat, a
// comparison against the least or greatest value, and a select at a
// constant index over stores at other constant indices and joins. It also
// shares terms: building the same term twice returns the same reference,
// so pointer equality is structural equality for terms built here.
//
// The terms it hands out are its own type, Term, which camada does not take:
// a bit-vector constant stays Terms' own value until the solver needs it,
// and solverTerm then makes the solver's term for it. Most constants fold
// away without ever getting one.
#pragma once
#include <camada/camada.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <deque>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace vereda::encode {

// A term of Terms: a bit-vector constant it interned, or a term of the
// solver. It does not convert to camada's handle, so every term that goes
// to camada passes through Terms::solverTerm. Its identity is one pointer,
// the constant's entry or the solver's term: copies of a term compare equal,
// and maps key a term by id().
class Term {
public:
  Term() = default;
  explicit operator bool() const { return key; }
  const void *id() const { return key; }
  bool operator==(const Term &o) const { return key == o.key; }
  bool isBool() const { return ref && ref->isBoolSort(); }
  unsigned width() const { return ref ? ref->getWidth() : entry()->value.getBitWidth(); }

private:
  friend class Terms;
  struct Constant {
    llvm::APInt value;
    mutable camada::SMTExprRef term; // the solver's, once made
  };
  explicit Term(const Constant *c) : key(c) {}
  explicit Term(camada::SMTExprRef r) : key(r.get()), ref(std::move(r)) {}
  // The constant's entry, or null for a solver term.
  const Constant *entry() const { return ref ? nullptr : static_cast<const Constant *>(key); }

  const void *key = nullptr; // the entry, or the solver's term
  camada::SMTExprRef ref;    // the solver's term; null for a constant
};

class Terms {
public:
  using Ref = Term;
  explicit Terms(camada::SMTSolver &s);
  camada::SMTSolver &solver() { return s; }

  // The value of a constant term (width 1 for Bool), or null.
  const llvm::APInt *constant(const Ref &e) const;
  bool isTrue(const Ref &e) const { return e == bools[1]; }
  bool isFalse(const Ref &e) const { return e == bools[0]; }
  // e as camada takes it: for a bit-vector constant the solver's term, made
  // here the first time one is needed. The only way from a Term to camada.
  camada::SMTExprRef solverTerm(const Ref &e);
  // A term camada gave back: the solver's term for a constant becomes the
  // constant again (mkIEEEFPToBV gives back the bits a float was made
  // from), so folding still sees it.
  Ref fromSolver(camada::SMTExprRef e) const;

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
    return Ref(s.mkSymbol(name, sort));
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
    const void *a, *b, *c; // ids
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
  camada::SMTExprRef build(const Node &n);
  // n's operation applied to other operands.
  Ref rebuild(const Node &n, const Ref &a, const Ref &b, const Ref &c);
  // def(index) of the lambda l, once per index.
  Ref reduce(const Ref &l, const Ref &index);

  camada::SMTSolver &s;
  std::unordered_map<Key, Ref, KeyHash> memo;
  // The maps below key a term by its id. Node-based maps: folding recurses
  // while holding references into them.
  std::unordered_map<const void *, Node> nodes;
  // The constants' entries, which stay where they are, and each by value.
  std::deque<Term::Constant> constants;
  llvm::DenseMap<llvm::APInt, const Term::Constant *> bvs;
  // Per solver term made for a constant, the constant.
  llvm::DenseMap<const camada::SMTExpr *, Ref> made;
  Ref bools[2];
  unsigned joinDepth = 0; // nesting of the concat join, bounded
  struct PairHash {
    size_t operator()(const std::pair<const void *, const void *> &p) const {
      return std::hash<const void *>()(p.first) * 31 + std::hash<const void *>()(p.second);
    }
  };
  std::unordered_map<std::pair<const void *, const void *>, bool, PairHash> implied;
  bool entails(const Ref &guard, const Ref &lit, unsigned depth);
  std::unordered_map<const void *, bool> arith;
  // Constant arrays built here (kept alive, so the key stays theirs) and
  // the value each holds.
  std::unordered_map<const void *, std::pair<Ref, Ref>> filled;
  std::unordered_map<const void *, std::function<Ref(const Ref &)>> lambdas;
  // Per store or choice of arrays, the lambdas it reads through without
  // passing another one.
  std::unordered_map<const void *, std::vector<Ref>> under;
  // Per lambda and index, the index (kept alive) and the definition there.
  std::unordered_map<std::pair<const void *, const void *>, std::pair<Ref, Ref>, PairHash>
      reduced;
  // Per array and constant index, what select gave: a read walks up to 256
  // stores, and a load of an element reads it once per byte.
  std::unordered_map<std::pair<const void *, const void *>, Ref, PairHash> selects;
  // Per symbol, the terms found not to contain it.
  std::unordered_map<const void *, std::unordered_set<const void *>> freeOf;
};

} // namespace vereda::encode
