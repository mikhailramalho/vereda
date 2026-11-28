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
#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <bit>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <new>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
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
  // Calls f(x, y) for each term x that cond being true, or false when
  // !holds, makes equal to a bitvector y: an x = y found through the
  // conjuncts of cond, the disjuncts of its negation, and negations. y is
  // the constant when one side is; x is not a constant. An extension equal
  // to a constant it can take also gives its operand that constant's low
  // bits: zext(x) = 5 is x = 5. Only Terms' own = counts, which is equality
  // of the terms: never a float's fp.eq (+0 equals -0) nor anything opaque.
  void equalities(const Ref &cond, bool holds,
                  llvm::function_ref<void(const Ref &, const Ref &)> f);
  // Whether e is the solver's own term, which Terms built no node for: a
  // symbol, or an operation Terms does not model.
  bool leaf(const Ref &e) const { return e && !e.entry() && !node(e); }
  // e as ite(c, x, y) with constant x and y.
  bool split(const Ref &e, Ref &c, Ref &x, Ref &y) const;
  // e as ite(c, x, y), whatever x and y are.
  bool choice(const Ref &e, Ref &c, Ref &x, Ref &y) const;
  // One choice in an address sum, distributed over its additions.
  bool sumChoice(const Ref &e, Ref &c, Ref &x, Ref &y, unsigned depth = 0);
  // g past p: c when g is land(p, c), as guards grow; else g.
  Ref past(const Ref &g, const Ref &p) const;
  // e as add(a, b) or sub(a, b).
  bool isAdd(const Ref &e, Ref &a, Ref &b) const;
  bool isSub(const Ref &e, Ref &a, Ref &b) const;
  // e as store(a, i, v).
  bool stored(const Ref &e, Ref &a, Ref &i, Ref &v) const;
  // The value every index of e holds, if e is a constant array; else null.
  Ref filledWith(const Ref &e) const;
  // Whether e multiplies, divides or takes a remainder, outside of arrays.
  bool arithmetic(const Ref &e);

  // What a path's conditions bound a term of at most 64 bits to: its least
  // and greatest value as an unsigned and as a signed number, and up to
  // MaxExcluded values inside them that it is not (as unsigned numbers).
  struct Bounds {
    uint64_t ulo, uhi;
    int64_t slo, shi;
    llvm::SmallVector<uint64_t, 2> excluded = {};
    bool operator==(const Bounds &) const = default;
  };
  static constexpr unsigned MaxExcluded = 8;
  // The bounds a path knows a term by, if any (encoder.h keeps them).
  using Known = llvm::function_ref<std::optional<Bounds>(const Ref &)>;
  // The values a term of at most 64 bits may take: lo + stride * i for
  // i = 0, 1, ... up to hi, with no wrap past its width (a strided
  // interval, as in value-set analysis). Known for constants, symbols given
  // a bound, and add, sub, mul, logical shifts, extension, extract and
  // concat of those, as offsets are built; null for anything else.
  // Comparisons fold by them.
  struct Range {
    uint64_t lo, hi, stride;
  };
  std::optional<Range> range(const Ref &e, Known known = {}) {
    if (known)
      clearScratch();
    return rangeOf(e, known);
  }
  // Whether cmp, an =, <, <= (unsigned or signed), an overflow test or the
  // negation of one, holds for every value the ranges of its sides take
  // under known (1), for none (0), or neither (-1). An overflow test is
  // settled only when it cannot hold.
  int decide(const Ref &cmp, Known known);
  // Calls set(x, b) for each term x that cond being true, or false when
  // !holds, bounds tighter than its range under known: x on one side of a
  // <, <= (unsigned or signed) or its negation, found as equalities finds
  // an =, bounded by the range of the other side; x on one side of an =,
  // bounded as the other side is; x on one side of a negated = with a
  // constant c on the other, which moves off c when c is an end of its
  // range and is excluded otherwise. A bound on an extension also bounds
  // its operand.
  void narrow(const Ref &cond, bool holds, Known known,
              llvm::function_ref<void(const Ref &, const Bounds &)> set);
  // e as known's path sees it: every comparison and overflow test decide
  // settles there is its constant, an ite whose condition it settles is
  // that side, and a read past a store whose index the read's is never
  // equal to (or always is) reads below it (or the value stored), through
  // not, and, or, ite, concat, extract and extensions, over at most a
  // budget of nodes. Equal to e only where known's bounds hold, so it is
  // never cached here; e itself when nothing folds.
  Ref assuming(const Ref &e, Known known);
  // Symbol x is below limit, at least 1, wherever a term holding it is
  // read: the index of a summarised loop's iteration, whose terms count only
  // under its guard.
  void bound(const Ref &x, uint64_t limit) { bounds.try_emplace(x.id(), x, limit); }
  // Whether e has at most cap distinct nodes, leaves included.
  bool within(const Ref &e, unsigned cap) const;

private:
  struct Node;
  // f(lit, holds) for each literal of cond (a node that is not a not, an
  // and, an or nor a Bool ite with a constant arm), as cond being true, or
  // false when !holds, makes it hold or fail, over at most 64 nodes, each
  // walked once, nearer the top first.
  void literals(const Ref &cond, bool holds,
                llvm::function_ref<void(const Node &, bool)> f) const;
  // The value at constant index i of arr, past stores at other constant
  // indices, which arr moves over, and through joins whose two sides hold
  // the same value there (as a stream pointer does after a branch that
  // stored elsewhere). Null when it is not found within steps nodes, a cap
  // so that a read past many stores stays a select.
  Ref read(Ref &arr, const llvm::APInt &i, unsigned &steps);
  // select without the cache below.
  Ref uncached(const Ref &array, const Ref &index);
  // decide without clearing scratch, for queries under one known.
  int decided(const Ref &cmp, Known known);
  // One node of assuming's walk, memoised in memo, over budget nodes.
  Ref assumed(const Ref &x, Known known, llvm::DenseMap<const void *, Ref> &memo,
              unsigned &budget);
  enum class Op : uint8_t {
    Add, Sub, Mul, UDiv, SDiv, URem, SRem, Shl, LShr, AShr, BVAnd, BVOr, BVXor,
    BVNot, Concat, Extract, ZExt, SExt, Eq, Ult, Ule, Slt, Sle, And, Or, Xor,
    Not, Ite, UAddO, SAddO, USubO, SSubO, UMulO, SMulO, Select, Store,
  };
  struct Node {
    Op op;
    Ref a, b, c;
    // Extract bounds or extension width; Store x is its parent entry plus one.
    unsigned x = 0, y = 0;
  };
  struct Entry {
    Node n;
    Ref self;
  };
  struct Key {
    Op op;
    const void *a, *b, *c; // ids
    unsigned x, y;
  };
  struct MemoBucket {
    static constexpr uint32_t Empty = ~uint32_t(0);
    uint32_t idx = Empty;
    uint32_t hash = 0;
  };
  static uint32_t hashKey(const Key &k);
  static bool matches(const Node &n, const Key &k) {
    return n.op == k.op && n.a.id() == k.a && n.b.id() == k.b && n.c.id() == k.c &&
           (n.op == Op::Store || n.x == k.x) && n.y == k.y;
  }
  Ref memoFind(const Key &k) const;
  void memoGrow();
  void memoInsert(uint32_t idx, uint32_t h);

  const Node *node(const Ref &e) const;
  Ref cancel(const Ref &sum, const Ref &part, unsigned &budget);
  Ref make(Node n);
  // Whether c (1) or its negation (-1) is a conjunct of g, found going up
  // g's left spine of ands for a bounded number of steps; 0 when neither is.
  int conjunct(const Ref &g, const Ref &c) const;
  // f pushed through an ite with constant branches, so that it folds on
  // each; null when no operand is one, or two have different conditions.
  // Pointers from an allocation that may fail, ite(ok, p, null), stay
  // constant on each side through offsets, comparisons and memory.
  template <typename F>
  Ref lift(const Ref &a, F &&f) {
    Ref c, x, y;
    return split(a, c, x, y) ? ite(c, f(x), f(y)) : Ref();
  }
  template <typename F>
  Ref lift(const Ref &a, const Ref &b, F &&f) {
    Ref ca, cb, ax = a, ay = a, bx = b, by = b;
    bool sa = split(a, ca, ax, ay), sb = split(b, cb, bx, by);
    if ((!sa && !sb) || (sa && sb && ca != cb) || (!sa && !constant(a)) ||
        (!sb && !constant(b)))
      return {};
    return ite(sa ? ca : cb, f(ax, bx), f(ay, by));
  }
  camada::SMTExprRef build(const Node &n);
  // n's operation applied to other operands.
  Ref rebuild(const Node &n, const Ref &a, const Ref &b, const Ref &c);
  // def(index) of the lambda l, once per index.
  Ref reduce(const Ref &l, const Ref &index);

  camada::SMTSolver &s;
  // Pointer-stable storage for built nodes, indexed by memoBuckets (open-
  // addressed hash-consing table) and nodes (id -> entry index).
  std::deque<Entry> entries;
  std::vector<MemoBucket> memoBuckets;
  uint32_t memoCount = 0;
  llvm::DenseMap<const void *, uint32_t> nodes;
  // The constants' entries, which stay where they are, and each by value.
  std::deque<Term::Constant> constants;
  llvm::DenseMap<llvm::APInt, const Term::Constant *> bvs;
  // Per solver term made for a constant, the constant.
  llvm::DenseMap<const camada::SMTExpr *, Ref> made;
  Ref bools[2];
  unsigned joinDepth = 0; // nesting of the concat join, bounded
  unsigned joinBudget = 0; // remaining ite splits in the current concat
  llvm::DenseMap<std::pair<const void *, const void *>, bool> implied;
  bool entails(const Ref &guard, const Ref &lit, unsigned depth, unsigned &budget);
  llvm::DenseMap<const void *, bool> arith;
  // Constant arrays built here (kept alive, so the key stays theirs) and
  // the value each holds, plus memoisation by (index sort, value).
  llvm::DenseMap<const void *, std::pair<Ref, Ref>> filled;
  llvm::DenseMap<std::pair<const void *, const void *>, Ref> constArrays;
  struct Lambda {
    Def def;
    Ref below;
    std::function<bool(const Ref &)> misses;
  };
  std::unordered_map<const void *, Lambda> lambdas;
  // At most two descendants covering each array's reachable lambdas. Forward
  // their children when the union fits, otherwise keep the two choice arms.
  llvm::DenseMap<const void *, llvm::SmallVector<Ref, 2>> under;
  // (array, index) pairs whose reachable lambdas have already been constrained
  // at index.
  llvm::DenseSet<std::pair<const void *, const void *>> constrained;
  // Per lambda and index, the index (kept alive) and the definition there.
  llvm::DenseMap<std::pair<const void *, const void *>, std::pair<Ref, Ref>> reduced;
  // Per array and constant index, what select gave: a read walks up to 256
  // stores, and a load of an element reads it once per byte.
  llvm::DenseMap<std::pair<const void *, const void *>, Ref> selects;
  // rangeOf: range, with each term also narrowed to the bounds known gives it.
  // Such a range holds only on known's path, so it is not kept, and nothing
  // folds by it but decide.
  std::optional<Range> rangeOf(const Ref &e, Known known);
  // The shape of e, a node, after those of the operands shape reads,
  // each cached in ranges, or scratch under known, as it is found.
  std::optional<Range> shapes(const Ref &e, Known known);
  // Whether a < b, or a <= b, holds for every value range (or srange, if
  // sign) gives them (1), for none (0), or neither (-1).
  int order(const Ref &a, const Ref &b, bool orEqual, bool sign, Known known = {});
  // The least and greatest value of e as signed numbers: from its range
  // when that does not cross the sign bit, the bounds known gives it, and
  // the operand it sign-extends; null when there are none (a path no
  // execution takes) or e is wider than 64 bits.
  std::optional<std::pair<int64_t, int64_t>> srange(const Ref &e, Known known);
  // The range of n, a term of width bits, from its operands' (range's work).
  std::optional<Range> shape(const Node &n, unsigned width, Known known);
  // range's results for one query under known, cleared per query (a
  // DenseMap shrinks when it clears, so a large query leaves no cost).
  llvm::DenseMap<const void *, std::optional<Range>> scratch;
  void clearScratch() {
    scratch.clear();
    sscratch.clear();
  }
  // How many low bits of e, a sum or product, are known to be zero.
  unsigned zeros(const Ref &e);
  // zeros' results for sums and products, kept as ranges are.
  llvm::DenseMap<const void *, unsigned> zeroBits;
  llvm::DenseMap<const void *, std::pair<Ref, uint64_t>> bounds;
  llvm::DenseMap<const void *, std::optional<Range>> ranges;
  // srange's results, kept as range's are: on every path in sranges, under
  // one query's known in sscratch, cleared with scratch.
  llvm::DenseMap<const void *, std::optional<std::pair<int64_t, int64_t>>> sranges, sscratch;
  // Per symbol, the terms found not to contain it (unique_ptr keeps each
  // set stable across recursive substitute calls).
  llvm::DenseMap<const void *, std::unique_ptr<llvm::DenseSet<const void *>>> freeOf;
  uint64_t epoch = 0;
  bool walkDone = false;
};

// r, a check's answer from s, except that a std::bad_alloc thrown inside the
// check, which camada turns into an unknown answer, is thrown again: memory
// ran out, and the next query would run on a solver left half updated (#399).
// With mimalloc, its error hook ends the run at the failing allocation first.
inline camada::CheckResult checked(camada::SMTSolver &s, camada::CheckResult r) {
  if (r == camada::CheckResult::UNKNOWN && s.reasonUnknownDetail() == "std::bad_alloc")
    throw std::bad_alloc();
  return r;
}

} // namespace vereda::encode
