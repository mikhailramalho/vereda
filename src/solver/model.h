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
  Model(camada::SMTSolver &s, encode::Terms &terms) : s(s), terms(terms) {}

  // e's value, width 1 for a Bool and the IEEE bits for a float, or null
  // when the model has none.
  const llvm::APInt *value(const encode::Term &e) {
    if (const llvm::APInt *c = terms.constant(e))
      return c;
    auto [it, fresh] = values.try_emplace(e.id());
    if (fresh) {
      camada::SMTExprRef x = terms.solverTerm(e);
      if (x->isBoolSort()) {
        if (auto b = s.getBool(x))
          it->second = llvm::APInt(1, b.value());
      } else if (auto bits = x->isFPSort() ? s.getFPInBin(x) : s.getBVInBin(x)) {
        it->second = llvm::APInt(bits.value().size(), bits.value(), 2);
      }
    }
    return it->second ? &*it->second : nullptr;
  }

  // Whether the model reaches a step with this guard.
  bool reaches(const encode::Term &guard) {
    const llvm::APInt *v = value(guard);
    return v && v->isOne();
  }

private:
  camada::SMTSolver &s;
  encode::Terms &terms;
  std::unordered_map<const void *, std::optional<llvm::APInt>> values; // by Term id
};

} // namespace vereda::solver
