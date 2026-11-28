// Trailing zero bits through deep chains of sums, past any fixed depth of
// 32: d(k+1) = d(k) + (d(k) - 8) from x * 8 keeps three low zero bits at
// each of 40 levels, so its low three bits fold to 0 while its fourth bit
// (1 from the first level on) does not; e(k+1) = e(k) + e(k) from y * 8,
// 40 levels, folds the same way in linear time although each level reads
// e(k) twice (2^40 paths).
#undef NDEBUG
#include "encode/terms.h"
#include "solver/solver.h"
#include <cassert>

using vereda::encode::Term;
using vereda::encode::Terms;

int main() {
  auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
  assert(solver);
  Terms t(**solver);
  auto word = (*solver)->mkBVSort(64);
  Term d = t.mul(t.symbol("x", word), t.bv(8, 64));
  for (int i = 0; i < 40; ++i)
    d = t.add(d, t.sub(d, t.bv(8, 64)));
  assert(t.extract(d, 2, 0) == t.bv(0, 3));
  assert(t.extract(d, 3, 0) != t.bv(0, 4));

  Term e = t.mul(t.symbol("y", word), t.bv(8, 64));
  for (int i = 0; i < 40; ++i)
    e = t.add(e, e);
  assert(t.extract(e, 2, 0) == t.bv(0, 3));
  return 0;
}
