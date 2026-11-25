// Constant-index reads preserve stores, joins, symbolic barriers and old arrays.
#undef NDEBUG
#include "encode/terms.h"
#include "solver/solver.h"
#include <cassert>
#include <vector>

using vereda::encode::Term;
using vereda::encode::Terms;

int main() {
  auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
  assert(solver);
  Terms t(**solver);
  auto indexSort = (*solver)->mkBVSort(64), valueSort = (*solver)->mkBVSort(32);
  auto arraySort = (*solver)->mkArraySort(indexSort, valueSort);
  Term original = t.symbol("original", arraySort), a = original;
  std::vector<Term> arrays{a};
  for (unsigned i = 0; i < 600; ++i) {
    Term next = t.store(a, t.bv(i, 64), t.bv(3 * i + 1, 32));
    assert(t.store(a, t.bv(i, 64), t.bv(3 * i + 1, 32)) == next);
    arrays.push_back(a = next);
  }
  auto equal = [&](const Term &x, const Term &y) {
    assert((*solver)->checkSatAssuming({t.solverTerm(t.lnot(t.eq(x, y)))}) ==
           camada::CheckResult::UNSAT);
  };
  // Reads inside and beyond the 256-node budget, plus the opaque-array tail.
  for (unsigned i : {0u, 1u, 343u, 344u, 345u, 599u}) {
    Term value = t.select(a, t.bv(i, 64));
    equal(value, t.bv(3 * i + 1, 32));
    assert(t.select(a, t.bv(i, 64)) == value);
  }
  const auto *edge = t.constant(t.select(a, t.bv(344, 64)));
  assert(edge && *edge == llvm::APInt(32, 1033));
  assert(!t.constant(t.select(a, t.bv(343, 64))));
  equal(t.select(a, t.bv(700, 64)), t.select(original, t.bv(700, 64)));
  equal(t.select(arrays[100], t.bv(99, 64)), t.bv(298, 32));
  Term overwritten = t.store(a, t.bv(0, 64), t.bv(17, 32));
  equal(t.select(overwritten, t.bv(0, 64)), t.bv(17, 32));
  equal(t.select(a, t.bv(0, 64)), t.bv(1, 32));

  Term k = t.symbol("k", indexSort), v = t.symbol("v", valueSort);
  Term symbolic = t.store(a, k, v);
  equal(t.select(symbolic, t.bv(0, 64)), t.ite(t.eq(k, t.bv(0, 64)), v, t.bv(1, 32)));
  Term c = t.symbol("c", (*solver)->mkBoolSort());
  Term joined = t.ite(c, t.store(a, t.bv(0, 64), t.bv(17, 32)),
                        t.store(a, t.bv(1, 64), t.bv(19, 32)));
  equal(t.select(t.store(joined, t.bv(2, 64), t.bv(23, 32)), t.bv(0, 64)),
        t.ite(c, t.bv(17, 32), t.bv(1, 32)));
  equal(t.select(joined, t.bv(599, 64)), t.bv(1798, 32));
  Term filled = t.constArray(indexSort, t.bv(31, 32));
  equal(t.select(t.store(filled, t.bv(1, 64), v), t.bv(2, 64)), t.bv(31, 32));

  // A backend rewrite can make a formerly opaque array acquire a node later.
  Term tail = t.symbol("tail", arraySort);
  Term before = t.store(tail, t.bv(1, 64), t.bv(41, 32));
  Term old = t.select(tail, t.bv(0, 64));
  Term alias = t.store(tail, t.bv(0, 64), old);
  equal(alias, tail);
  equal(t.select(before, t.bv(0, 64)), old);
  assert(t.store(tail, t.bv(1, 64), t.bv(41, 32)) == before);
}
