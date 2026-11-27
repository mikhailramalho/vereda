// Forwarding array dependencies must retain every lambda definition after joins.
#undef NDEBUG
#include "encode/terms.h"
#include "solver/solver.h"
#include <cassert>
#include <vector>

using vereda::encode::Term;
using vereda::encode::Terms;

int main() {
  for (unsigned count : {1u, 2u, 4u}) {
    auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
    assert(solver);
    Terms t(**solver);
    auto bits = (*solver)->mkBVSort(8), arraySort = (*solver)->mkArraySort(bits, bits);
    Term x = t.symbol("index", bits);
    std::vector<Term> arrays, values;
    for (unsigned i = 0; i < count; ++i) {
      auto definition = [&, i](const Term &at) {
        return t.add(t.mul(at, t.bv(i + 1, 8)), t.bv(11 * i, 8));
      };
      arrays.push_back(t.lambda(arraySort, "lambda" + std::to_string(i), definition));
      values.push_back(definition(x));
    }
    // Four leaves force the bounded two-child representation to retain a DAG.
    for (unsigned width = count; width > 1; width /= 2)
      for (unsigned i = 0; i < width / 2; ++i) {
        Term c = t.symbol("join" + std::to_string(width) + ":" + std::to_string(i),
                          (*solver)->mkBoolSort());
        arrays[i] = t.ite(c, arrays[2 * i], arrays[2 * i + 1]);
        values[i] = t.ite(c, values[2 * i], values[2 * i + 1]);
      }
    Term a = arrays[0], expected = values[0];
    for (unsigned i = 0; i < 4; ++i) {
      Term c = t.symbol("write" + std::to_string(i), (*solver)->mkBoolSort());
      Term at = t.bv(i, 8), value = t.bv(100 + i, 8);
      a = t.ite(c, t.store(a, at, value), a);
      expected = t.ite(t.land(c, t.eq(x, at)), value, expected);
    }
    Term opaque = t.symbol("opaque", arraySort);
    Term choose = t.symbol("choose", (*solver)->mkBoolSort());
    a = t.store(t.ite(choose, a, opaque), t.bv(100, 8), t.bv(7, 8));
    expected = t.ite(t.eq(x, t.bv(100, 8)), t.bv(7, 8),
                     t.ite(choose, expected, t.select(opaque, x)));
    Term same = t.eq(t.select(a, x), expected);
    assert((*solver)->checkSatAssuming({t.solverTerm(t.lnot(same))}) ==
           camada::CheckResult::UNSAT);
    a = t.store(a, t.bv(200, 8), t.bv(9, 8));
    expected = t.ite(t.eq(x, t.bv(200, 8)), t.bv(9, 8), expected);
    same = t.eq(t.select(a, x), expected);
    assert((*solver)->checkSatAssuming({t.solverTerm(t.lnot(same))}) ==
           camada::CheckResult::UNSAT);
  }
}
