// The equalities of a condition whose subterms are shared, as path guards
// are: L(k+1) = (L(k) and q(k)) and (L(k) and r(k)) has 2^k paths but five
// new nodes per level. An equality on the right of a 14-level L is found
// although L alone has more nodes than the walk's budget of 64, and one at
// the bottom of a 10-level L (51 nodes) is found although it sits 20
// levels down.
#undef NDEBUG
#include "encode/terms.h"
#include "solver/solver.h"
#include <cassert>
#include <string>

using vereda::encode::Term;
using vereda::encode::Terms;

int main() {
  auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
  assert(solver);
  Terms t(**solver);
  auto word = (*solver)->mkBVSort(32);
  int fresh = 0;
  auto lit = [&] {
    std::string n = std::to_string(fresh++);
    return t.ult(t.symbol("a" + n, word), t.symbol("b" + n, word));
  };
  auto shared = [&](Term l, int levels) {
    for (int i = 0; i < levels; ++i)
      l = t.land(t.land(l, lit()), t.land(l, lit()));
    return l;
  };
  auto finds = [&](const Term &cond, const Term &x, uint64_t v) {
    bool found = false;
    t.equalities(cond, true, [&](const Term &y, const Term &c) {
      found |= y == x && c == t.bv(v, 32);
    });
    return found;
  };

  Term z = t.symbol("z", word);
  assert(finds(t.land(shared(lit(), 14), t.eq(z, t.bv(9, 32))), z, 9));

  Term y = t.symbol("y", word);
  assert(finds(shared(t.eq(y, t.bv(7, 32)), 10), y, 7));
  return 0;
}
