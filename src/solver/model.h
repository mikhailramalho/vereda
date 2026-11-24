// Stage solver: the model's values of terms, for the counterexample and the
// witness.
#pragma once
#include "encode/terms.h"
#include <camada/camada.h>
#include <llvm/ADT/APInt.h>
#include <optional>
#include <unordered_map>

namespace vereda::solver {

// Reads each term's value once, and a constant the term builder folded from
// the builder, not the solver. A long trace is mostly a concrete loop, whose
// guards are few and whose values are constants, while Bitwuzla preprocesses
// the term again on every query: 100,000 iterations took seconds of queries.
class Model {
public:
  // The solver must hold the model while this is used.
  Model(camada::SMTSolver &s, const encode::Terms &terms) : s(s), terms(terms) {}

  // e's value, width 1 for a Bool and the IEEE bits for a float, or null
  // when the model has none.
  const llvm::APInt *value(const camada::SMTExprRef &e) {
    if (const llvm::APInt *c = terms.constant(e))
      return c;
    auto [it, fresh] = values.try_emplace(e.get());
    if (fresh) {
      if (e->isBoolSort()) {
        if (auto b = s.getBool(e))
          it->second = llvm::APInt(1, b.value());
      } else if (auto bits = e->isFPSort() ? s.getFPInBin(e) : s.getBVInBin(e)) {
        it->second = llvm::APInt(bits.value().size(), bits.value(), 2);
      }
    }
    return it->second ? &*it->second : nullptr;
  }

  // Whether the model reaches a step with this guard.
  bool reaches(const camada::SMTExprRef &guard) {
    const llvm::APInt *v = value(guard);
    return v && v->isOne();
  }

private:
  camada::SMTSolver &s;
  const encode::Terms &terms;
  std::unordered_map<const camada::SMTExpr *, std::optional<llvm::APInt>> values;
};

} // namespace vereda::solver
