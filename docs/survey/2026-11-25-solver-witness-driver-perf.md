# Solver, witness, and driver performance and memory optimizations

Date: 2026-11-25
Base commit: `afc20c02`

## Summary of changes

1. **Constant-false skip, check deduplication, and `--multi-property` disjunctive batching** (`src/solver/solver.cpp`):
   Checks whose violation term already folded to `Terms::isFalse` are marked `Status::Holds` with zero solver calls. Remaining checks in each phase (`Assertion`, then `Unwinding`/`Recursion`) are deduplicated by `violated.id()`. Under `--multi-property` (`all == true`), remaining groups are queried as a balanced binary `OR` tree in one `checkSatAssuming` call: `UNSAT` marks all remaining groups `Holds`, `SAT` extracts every `true` group from the model and removes them before repeating, and `UNKNOWN` falls back to checking groups individually.

   Disjunctive batching was also measured for single-property mode (`!all`) and **not adopted** there. Under Bitwuzla with `bv-cnf-scope=permanent`, combining unrelated checks into one `OR` forces Bitwuzla to bit-blast every check's cone of influence simultaneously:
   - `esbmc-unix2/23_picosat-846_01` (4 checks, check 0 fails): wall time regressed from 13.66 s to 34.80 s (2.5x) and peak memory from 378 MB to 733 MB (1.9x).
   - `vereda/lib_strtok` (9 safe checks under `--unwind 8`): solve time regressed from 5.28 s to 7.03 s and peak memory from 105 MB to 158 MB.

   Keeping deduplicated sequential solving for `!all` eliminates both regressions while preserving constant-false skipping, `violated.id()` deduplication, and exact first-failure/checked-prefix semantics.

2. **`solver::Model::assumeTrue(c.violated)`** (`src/solver/model.h`, `src/driver/main.cpp`):
   Caches `{c.violated.id(), APInt(1, 1)}` in `Model::values` right after constructing `Model` for a violated check, avoiding Bitwuzla re-evaluating the violated check term when `printCounterexample()` and `violationWitness()` reach the `Step::Check` step.

3. **Free `llvm::Module` before `solver::decide()` in single-walk mode** (`src/driver/main.cpp`, `src/witness/witness.{h,cpp}`):
   When `--witness` is set, `extractModuleInfo()` extracts a lightweight `WitnessModuleInfo` (compile-unit paths, pointer bitwidth, and declaration return kinds/sign attributes) before the walk. In `!incrementalBMC` mode, `module.reset()` is called right after `walk()` and `terms.releaseWalkTables()`, before `solver::decide()`, while keeping `LLVMContext` alive for `Step::type` (`const llvm::DIType *`).

4. **Counterexample and witness hot-path optimizations** (`src/witness/counterexample.cpp`, `src/witness/witness.cpp`):
   - `counterexample.cpp`: checks unchanged values (fast-pathing on `(Term::id(), isUnsigned)` before calling `model.value()`, and comparing normalized `(Kind, val)` before formatting strings), formats grouped bits for `<= 64`-bit values into a stack buffer, and caches `llvm::demangle(function)` in a local `StringMap<std::string>`.
   - `witness.cpp`: memoizes `keyword()` and `returnColumn()` per source site, replaces `quote()` with zero-allocation `writeQuoted(raw_ostream &, StringRef)`, and streams YAML metadata plus waypoint content directly to the output stream without copying `content` into a second buffer.
   - Verified byte-for-byte identical output (modulo UUID, timestamp, and timing lines) across all witness tests.

5. **Eliminate redundant `vereda --version` subprocess in `vereda.py`** (`vereda.py`, `src/driver/main.cpp`):
   Removed the extra `run([binary, "--version"])` spawn in `vereda.py`; `main.cpp` prints the version banner directly on normal runs (omitted when `--dump-ir-only` or `--dump-smt-only` writes to stdout, while `--version` continues to work unchanged). `physical_lines()` now uses `LINEMARKER.subn(b"", text)` to scan `.i` files once instead of twice.

## Measurements (`tests/bench.py`, `build/vereda-base` vs `build/vereda`)

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc-unix2/23_picosat-846_01 | 0.46 → 0.47 | 13.19 → 13.16 | 14.16 → 14.14 | 379 → 383 | FAILED | |
| esbmc/github_302 | 0.00 → 0.00 | 24.44 → 24.43 | 24.56 → 24.56 | 529 → 528 | FAILED | |
| esbmc/array_loop_summary_decrement_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.09 | 37 → 37 | FAILED | |
| esbmc/overflow_17_crc_new | 0.02 → 0.02 | 0.00 → 0.00 | 0.12 → 0.12 | 36 → 36 | SUCCESSFUL | |
| esbmc/overflow_18_insertsort_new | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.10 | 36 → 35 | SUCCESSFUL | |
| esbmc/overflow_22_wordplay | 0.01 → 0.01 | 0.55 → 0.55 | 0.70 → 0.69 | 97 → 98 | FAILED | |
| vereda/lib_strtok | 0.02 → 0.02 | 5.24 → 5.26 | 5.35 → 5.37 | 106 → 107 | SUCCESSFUL | |
| vereda/multi_property | 0.00 → 0.00 | 0.00 → 0.01 | 0.10 → 0.09 | 37 → 37 | FAILED | |
| vereda/witness_branching | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.10 | 39 → 40 | FAILED | |
| vereda/witness_violation | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.10 | 38 → 38 | FAILED | |

Targeted micro-benchmarks (median of 3 runs, wall time including clang):

| workload | base s | head s | speedup |
|---|---|---|---|
| `--multi-property` (200 const-false + 100 failing checks) | 0.126 | 0.116 | 1.09x |
| `--multi-property` (300 distinct UNSAT checks) | 0.138 | 0.131 | 1.06x |
| Long witness + counterexample (2,000 loop iterations) | 0.128 | 0.122 | 1.05x |
| `array_loop_summary_decrement_fail --no-summarize-array-loops` | 5.318 | 4.758 | 1.12x |
