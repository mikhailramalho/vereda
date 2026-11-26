// Exhaustively compare compact functions with independently constructed tables.
#undef NDEBUG
#include "memory/table.h"
#include "solver/solver.h"
#include <cassert>
#include <string>
#include <vector>

using vereda::encode::Term;
using vereda::encode::Terms;
using llvm::APInt;

static APInt bits(unsigned w, uint64_t value) {
  return APInt(64, value).zextOrTrunc(w);
}

static void verify(Terms &t, const std::vector<Term> &values) {
  auto f = vereda::memory::tableFunction(t, values);
  assert(f);
  for (size_t i = 0; i < values.size(); ++i) {
    Term d = t.sub(f(t.bv(i, 64)), values[i]);
    if (const APInt *c = t.constant(d))
      assert(c->isZero());
    else
      assert(t.solver().checkSatAssuming({t.solverTerm(t.lnot(t.eq(d, t.bv(0, d.width()))))}) ==
             camada::CheckResult::UNSAT);
  }
}

int main() {
  auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
  assert(solver);
  Terms t(**solver);
  for (unsigned w : {8u, 16u, 32u, 64u}) {
    Term x = t.symbol("x" + std::to_string(w), (*solver)->mkBVSort(w));
    Term y = t.symbol("y" + std::to_string(w), (*solver)->mkBVSort(w));
    std::vector<Term> values;
    for (unsigned i = 0; i < 256; ++i)
      values.push_back(i & 1 ? t.add(t.bv(bits(w, 3 * i)), x)
                            : t.add(x, t.bv(bits(w, 3 * i))));
    verify(t, values);
    // The first, middle and last elements are exceptions to a symbolic base.
    values[0] = y;
    values[100] = t.bv(17, w);
    values[255] = t.add(y, t.bv(1, w));
    verify(t, values);

    // More than eight scattered exceptions must retain the original array.
    values.clear();
    for (unsigned i = 0; i < 256; ++i)
      values.push_back(t.bv(bits(w, 5 + 3 * i)));
    for (unsigned i = 0; i < 9; ++i)
      values[1 + 23 * i] = t.add(values[1 + 23 * i], t.bv(1, w));
    assert(!vereda::memory::tableFunction(t, values));

    // An odd second difference needs w+1 index bits even after index 2^w.
    values.clear();
    for (uint64_t i = 0; i < 1024; ++i) {
      APInt value = bits(w, 7 + 3 * i + i * (i - 1) / 2);
      values.push_back(t.add(x, t.bv(value)));
    }
    verify(t, values);
    values.back() = t.add(values.back(), t.bv(1, w));
    assert(!vereda::memory::tableFunction(t, values));

    for (unsigned period = 2; period <= 15; ++period) {
      std::vector<Term> pattern;
      for (unsigned i = 0; i < period; ++i)
        pattern.push_back(t.symbol("p" + std::to_string(w) + "_" + std::to_string(i),
                                   (*solver)->mkBVSort(w)));
      values.clear();
      for (unsigned i = 0; i < 257; ++i)
        values.push_back(pattern[i % period]);
      verify(t, values);
      values.back() = y;
      assert(!vereda::memory::tableFunction(t, values));
    }

    values.clear();
    for (unsigned i = 0; i < 256; ++i) {
      if (i < 64)
        values.push_back(t.add(x, t.bv(3 * i, w)));
      else if (i < 192)
        values.push_back(t.sub(y, t.bv(bits(w, 5 * (i - 64)))));
      else
        values.push_back(x);
    }
    verify(t, values);

    values.clear();
    for (unsigned i = 0; i < 9 * 16; ++i)
      values.push_back(t.bv(bits(w, 11 + 37 * (i / 16))));
    assert(!vereda::memory::tableFunction(t, values));
  }
}
