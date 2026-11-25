# Walker, loop analysis, and IR preparation hot paths

Recorded 2026-11-25.

Five correctness-preserving optimizations targeting hot paths across `ir`,
`analysis`, and `walker`:

## Design

1. **Precomputed per-loop `regionBlocks` in `FunctionInfo`**
   (`src/walker/walker.cpp`):
   Previously `Walk::region(frame, fi, l)` scanned the entire function's `rpo`
   list on every iteration of every loop (`O(K * |F|)`), filtering by
   `l->contains(bb)` and walking `getParentLoop()`. `FunctionInfo`'s
   constructor now makes a single linear pass over `rpo` to build
   `DenseMap<const Loop *, SmallVector<BasicBlock *, 8>> regionBlocks`, where
   `regionBlocks[nullptr]` holds top-level blocks and top-level loop headers in
   RPO, and `regionBlocks[l]` holds only `l`'s direct blocks and immediate
   child-loop headers in RPO. `Walk::region` iterates only `regionBlocks[l]`
   (`O(K * |L|)`), and `FunctionInfo` no longer retains `rpo`. In `Walk::block`,
   `frame.arrivals.find(&bb)` returns early when absent instead of
   default-inserting and erasing an empty entry.

2. **Module-wide `TargetLibraryInfoImpl` and lazy `analysis::Loops`**
   (`src/walker/walker.cpp`, `src/analysis/loops.h`, `src/analysis/loops.cpp`):
   `Walk` now constructs one `TargetLibraryInfoImpl` and checks `pthread_create`
   once per module instead of once per function. `FunctionInfo::loops` is a
   lazy `std::unique_ptr<analysis::Loops>` allocated only when `Walk::loop()` is
   entered, skipping `AssumptionCache` and `ScalarEvolution` on all loop-free
   functions. In `Loops::callOk`, `depth >= 4 || f->isVarArg()` is checked
   before `FindFunctionBackedges`, and results are memoized per
   `PointerIntPair<const Function *, 1, bool>` `(callee, reads)`.

3. **Hot-path caching and lazy formatting**
   (`src/walker/walker.cpp`, `src/ir/source.cpp`):
   - `Walk::call` builds `tag` via a lazy lambda so `name.str() + "@" +
     ir::location(c).str()` is only formatted in the three branches that use it
     (`__VERIFIER_nondet_*`, `scanf`, and non-void unknown externals).
   - `ir::callArgumentText` caches extracted call arguments per `(file, line,
     column)` inside its file cache, avoiding repeated line scans when
     `__VERIFIER_assume` or `__VERIFIER_assert` executes in a loop.
   - `Walk::namedObject` memoizes debug-info lookups by stripped base pointer
     `const Value *` and returns a pointer to the cached entry.
   - `Walk::loop` hoists `loopLocation(l).display()` out of the unrolling loop.

4. **Early break on `guard == false` and constant `SwitchInst` fast path**
   (`src/walker/walker.cpp`):
   - `Walk::block` breaks immediately after `guard = call(*c, guard, st)` when
     `t.isFalse(guard)` (e.g. after `__VERIFIER_assume(0)`, `abort()`, or a
     non-returning callee), avoiding walking dead tail instructions and dead
     returns.
   - `Walk::function` returns an empty `Frame` immediately when `t.isFalse(guard)`.
   - `Walk::terminator` fast-paths `SwitchInst` when `t.constant(cond)` is
     known, jumping directly via `sw->findCaseValue(...)` to the matching case
     or default destination without constructing case equalities or a
     `MapVector`.

5. **Reachability pruning from `entry` and precomputed `addressTaken` in
   `ir::load`** (`src/ir/load.h`, `src/ir/load.cpp`, `src/driver/main.cpp`):
   `main.cpp` passes `entry` into `ir::load`. When `entry` resolves to a defined
   function, `pruneUnreachable` runs a reachability BFS from `entry` plus
   `llvm.global_ctors`, `llvm.global_dtors`, `llvm.used`, and
   `llvm.compiler.used`, traversing instructions, constant operands, global
   initializers, aliases, and ifuncs, and erases unreachable functions, globals,
   aliases, and ifuncs before `nondetLocals`, `lowerLongjmp`, `lowerCalls`,
   `linkModels`, and the LLVM pass pipeline. In `lowerCalls`, the vector of
   address-taken functions is computed once lazily on the first indirect call
   rather than calling `hasAddressTaken` per function per indirect call.

## Measurements

Side-by-side comparison with `tests/bench.py` against `afc20c02` (clang 23.1.6,
RelWithDebInfo):

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc-unix2/23_picosat-846_01 | 0.53 → 0.52 | 10.79 → 6.31 | 11.86 → 7.36 | 379 → 379 | FAILED | wall time x0.62 |
| esbmc/github_302 | 0.00 → 0.00 | 26.48 → 26.74 | 26.62 → 26.87 | 531 → 529 | FAILED | |
| esbmc/deep_binary_chain_pass | 0.00 → 0.00 | 0.00 → 0.00 | 19.54 → 19.20 | 51 → 47 | SUCCESSFUL | |
| esbmc/array_loop_summary | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 37 → 38 | SUCCESSFUL | |
| esbmc/array_loop_summary_unroll | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.10 | 35 → 34 | SUCCESSFUL | |
| esbmc/array_loop_summary_store | 0.00 → 0.00 | 0.08 → 0.08 | 0.19 → 0.18 | 40 → 39 | SUCCESSFUL | |
| vereda/fnptr_call | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.11 | 36 → 36 | SUCCESSFUL | |
| vereda/fnptr_table | 0.00 → 0.00 | 0.15 → 0.14 | 0.25 → 0.25 | 45 → 45 | SUCCESSFUL | |
| vereda/fnptr_types | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 36 → 36 | FAILED | |

Additional unrolled loop walk timings with `--no-summarize-array-loops`:

- `tests/esbmc/array_loop_summary.c`: walk time 1.879 s → 1.630 s (-13.3%).
- `tests/esbmc/array_loop_summary_store.c` (`--dump-smt-only`): walk time
  2.023 s → 1.909 s (-5.6%).
- `tests/esbmc-unix2/23_picosat-846_01` (`--unwind 1`): symbols dropped from 84
  to 62 after unreachable function and global pruning.

Full `ctest --test-dir build -j10 --output-on-failure`: 6943 tests, 100% passed,
0 failed (162.41 s real), including 8 new unit tests under `tests/vereda/`.
