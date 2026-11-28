// Address recovery preserves object identity and offsets at every alignment boundary.
#undef NDEBUG
#include "encode/terms.h"
#include "memory/memory.h"
#include "solver/solver.h"
#include <llvm/IR/DataLayout.h>
#include <cassert>

using vereda::encode::Term;
using vereda::encode::Terms;

int main() {
  for (unsigned bits : {32u, 64u})
    for (uint64_t align : {1u, 2u, 4u, 16u, 64u})
      for (uint64_t size : {uint64_t(0), uint64_t(1), align - 1, align, align + 1}) {
        auto solver = vereda::solver::create(camada::SolverKind::Bitwuzla);
        assert(solver);
        Terms t(**solver);
        llvm::DataLayout dl(bits == 32 ? "e-p:32:32" : "e-p:64:64");
        vereda::memory::Memory mem(t, dl);
        Term exists = t.symbol("exists", (*solver)->mkBoolSort());
        Term p = mem.allocate(t.bv(size, bits), align, exists);
        Term base = mem.address(p);
        Term a = t.symbol("address", (*solver)->mkBVSort(bits));
        Term recovered = mem.fromAddress(a);
        Term offset = t.sub(a, base);
        Term inside = t.land(exists, t.ule(offset, t.bv(size, bits)));
        Term expected = t.ite(inside, mem.ptr(mem.object(p), offset),
                             mem.ptr(t.bv(0, mem.object(p).width()), a));
        assert((*solver)->checkSatAssuming({t.solverTerm(t.lnot(t.eq(recovered, expected)))}) ==
               camada::CheckResult::UNSAT);
      }
}
