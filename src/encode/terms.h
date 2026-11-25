// Stage encode: the term builder every stage uses in place of camada's
// constructors. It folds as terms are built (plan, section 4.2): constants,
// identities, an ite with a constant guard, extract over concat, a
// comparison against the least or greatest value or that the ranges of its
// sides settle, and a select at a constant index over stores at other
// constant indices and joins. It also
// shares terms: building the same term twice returns the same reference,
// so pointer equality is structural equality for terms built here. What
// holds only on one path (the bounds a path's conditions give its terms)
// never folds a term here: decide answers for that path alone.
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
#include <bit>
#include <cstdint>
#include <deque>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace vereda::encode {

// A term of Terms: a bit-vector constant it interned, or a term of the
// solver. It does not convert to camada's handle, so every term that goes
// to camada passes through Terms::solverTerm. It is one pointer, the
// solver's handle or, with the low bit set, the constant's entry, so its
// identity is that pointer: copies of a term compare equal, and maps key a
// term by id().
class Term {
public:
  Term() = default;
  explicit operator bool() const { return bits; }
  const void *id() const { return reinterpret_cast<const void *>(bits); }
  bool operator==(const Term &o) const { return bits == o.bits; }
  bool isBool() const { return bits && !entry() && ref()->isBoolSort(); }
  bool isBV() const { return entry() || (bits && ref()->isBVSort()); }
  unsigned width() const { return entry() ? entry()->value.getBitWidth() : ref()->getWidth(); }

private:
  friend class Terms;
  struct Constant {
    llvm::APInt value;
    mutable camada::SMTExprRef term; // the solver's, once made
  };
  // A handle is one pointer only while camada's handles are unchecked, as in
  // its release archives; checked ones would make a Term twice the size.
  static_assert(sizeof(camada::SMTExprRef) == sizeof(uintptr_t) &&
                    std::is_trivially_copyable_v<camada::SMTExprRef>,
                "Term needs camada built with CAMADA_CHECKED_HANDLES=OFF");
  explicit Term(const Constant *c) : bits(reinterpret_cast<uintptr_t>(c) | 1) {}
  explicit Term(const camada::SMTExprRef &r) : bits(std::bit_cast<uintptr_t>(r)) {}
  // The constant's entry, or null for a solver term.
  const Constant *entry() const {
    return bits & 1 ? reinterpret_cast<const Constant *>(bits - 1) : nullptr;
  }
  // The solver's term; null for a constant.
  camada::SMTExprRef ref() const {
    return bits & 1 ? camada::SMTExprRef() : std::bit_cast<camada::SMTExprRef>(bits);
  }

  uintptr_t bits = 0; // both an entry and a solver term are aligned
};

class Terms {
public:
  using Ref = Term;
  explicit Terms(camada::SMTSolver &s);
  Terms(const Terms &) = delete; // its terms point into it
  camada::SMTSolver &solver() { return s; }
  // Adds a global constraint to the solver and advances constraintsEpoch()
  // so callers that cache SAT answers know to re-check them.
  void addConstraint(const Ref &c) {
    if (isTrue(c))
      return;
    s.addConstraint(solverTerm(c));
    ++epoch;
  }
  // Releases the hash-consing, folding and range caches once the walk is
  // done; only constant entries and solverTerm() remain valid afterwards.
  void releaseWalkTables();
  uint64_t constraintsEpoch() const { return epoch; }

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
  Ref fromSolver(const camada::SMTExprRef &e) const;

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
  // the solver sees the array only at the indices it is read at. A read at
  // an index i where misses(i) holds, at which def(i) is below's element,
  // goes to below without a term: of a chain of lambdas, one per row of a
  // 2-D array, a read reaches only those of the rows it may be in.
  using Def = std::function<Ref(const Ref &)>;
  Ref lambda(const camada::SMTSortRef &sort, const std::string &name, Def def,
             const Ref &below = {}, std::function<bool(const Ref &)> misses = {});
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

  // What a path's conditions bound a term of at most 64 bits to: its least
  // and greatest value as an unsigned and as a signed number.
  struct Bounds {
    uint64_t ulo, uhi;
    int64_t slo, shi;
    bool operator==(const Bounds &) const = default;
  };
  // The bounds a path knows a term by, if any (encoder.h keeps them).
  using Known = llvm::function_ref<std::optional<Bounds>(const Ref &)>;
  // The values a term of at most 64 bits may take: lo + stride * i for
  // i = 0, 1, ... up to hi, with no wrap past its width (a strided
  // interval, as in value-set analysis). Known for constants, symbols given
  // a bound, and add, sub, mul, logical shift right, extension, extract and
  // concat of those, as offsets are built; null for anything else.
  // Comparisons fold by them.
  struct Range {
    uint64_t lo, hi, stride;
  };
  std::optional<Range> range(const Ref &e) { return range(e, {}, 0); }
  // Whether cmp, an =, <, <= (unsigned or signed) or the negation of one,
  // holds for every value the ranges of its sides take under known (1), for
  // none (0), or neither (-1).
  int decide(const Ref &cmp, Known known);
  // Calls set(x, b) for each term x that cond being true, or false when
  // !holds, bounds tighter than its range under known: x on one side of a
  // <, <= (unsigned or signed) or its negation, found as equalities finds
  // an =, bounded by the range of the other side.
  void narrow(const Ref &cond, bool holds, Known known,
              llvm::function_ref<void(const Ref &, const Bounds &)> set);
  // Symbol x is below limit, at least 1, wherever a term holding it is
  // read: the index of a summarised loop's iteration, whose terms count only
  // under its guard.
  void bound(const Ref &x, uint64_t limit) { bounds.try_emplace(x.id(), x, limit); }

private:
  struct Node;
  // f(lit, holds) for each literal of cond (a node that is not a not, an
  // and, an or nor a Bool ite with a constant arm), as cond being true, or
  // false when !holds, makes it hold or fail, over at most budget nodes.
  void literals(const Ref &cond, bool holds, llvm::function_ref<void(const Node &, bool)> f,
                unsigned &budget) const;
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
  struct Lambda {
    Def def;
    Ref below;
    std::function<bool(const Ref &)> misses;
  };
  std::unordered_map<const void *, Lambda> lambdas;
  // Per store or choice of arrays, the lambdas it reads through without
  // passing another one.
  std::unordered_map<const void *, std::vector<Ref>> under;
  // Per lambda and index, the index (kept alive) and the definition there.
  std::unordered_map<std::pair<const void *, const void *>, std::pair<Ref, Ref>, PairHash>
      reduced;
  // Per array and constant index, what select gave: a read walks up to 256
  // stores, and a load of an element reads it once per byte.
  std::unordered_map<std::pair<const void *, const void *>, Ref, PairHash>
      selects;
  // range, with each term also narrowed to the bounds known gives it. Such
  // a range holds only on known's path, so it is not kept, and nothing
  // folds by it but decide.
  std::optional<Range> range(const Ref &e, Known known, unsigned depth);
  // Whether a < b, or a <= b, holds for every value range (or srange, if
  // sign) gives them (1), for none (0), or neither (-1).
  int order(const Ref &a, const Ref &b, bool orEqual, bool sign, Known known = {});
  // The least and greatest value of e as signed numbers: from its range
  // when that does not cross the sign bit, the bounds known gives it, and
  // the operand it sign-extends; null when there are none (a path no
  // execution takes) or e is wider than 64 bits.
  std::optional<std::pair<int64_t, int64_t>> srange(const Ref &e, Known known, unsigned depth);
  // The range of n, a term of width bits, from its operands' (range's work).
  std::optional<Range> shape(const Node &n, unsigned width, Known known, unsigned depth);
  // range's results for one query under known, cleared per query (a
  // DenseMap shrinks when it clears, so a large query leaves no cost).
  llvm::DenseMap<const void *, std::optional<Range>> scratch;
  // How many low bits of e, a sum or product, are known to be zero.
  unsigned zeros(const Ref &e, unsigned depth = 0);
  std::unordered_map<const void *, std::pair<Ref, uint64_t>> bounds;
  llvm::DenseMap<const void *, std::optional<Range>> ranges;
  // Per symbol, the terms found not to contain it.
  std::unordered_map<const void *, std::unordered_set<const void *>> freeOf;
  uint64_t epoch = 0;
  bool walkDone = false;
};

} // namespace vereda::encode
