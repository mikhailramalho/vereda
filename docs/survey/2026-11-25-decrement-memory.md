# Memory breakdown of the decrement test without loop summaries

Recorded 2026-11-25.

Issue #99. At `0f60211a` (`RelWithDebInfo`, mimalloc 3.7.0, LLVM 23.1.6, camada
0.25 with Bitwuzla 0.11.0), running `tests/esbmc/array_loop_summary_decrement_fail.c`
with `--no-summarize-array-loops` walked in 1.54 s at 393 MB peak RSS and
solved in 3.56 s at 782 MB peak RSS. This note records where that memory went,
the four changes adopted to cut it, and the comparison against ESBMC on the
passing test `array_loop_summary_decrement.c`.

## Breakdown at `0f60211a`

Instrumenting `Terms`, `Trace` and `Memory::flush` with `/proc/self/status`
(`VmRSS` / `VmHWM`) on `array_loop_summary_decrement_fail.c --no-summarize-array-loops`
gave:

- **Before `flush` (`known.size() == 400,000`)**: `VmRSS` 308 MiB, `VmHWM` 356 MiB.
  - `Trace` (`std::vector<Step>`): 600,009 steps (500,005 `Assignment`, 100,001
    `Branch`), `sizeof(Step) == 152` bytes, capacity doubled to 1,048,576
    (159.4 MB reserved, ~91 MB touched RSS, plus a ~50 MB transient spike on the
    512K→1M reallocation) plus 28.8 MB of per-step heap copies of `loc.file`
    (`"tests/esbmc/array_loop_summary_decrement_fail.c"`). Total: ~120 MB live
    RSS, ~188 MB allocated.
  - `CellMap` (`st.known`): 400,000 byte leaves + 399,999 internal branch nodes
    as `std::shared_ptr<const Node>` (~128 B each including the control block):
    ~102 MB.
  - `Terms`: 475,258 interned constants (100,000 of them 96-bit `APInt`s with
    heap storage) in `constants` (~13.0 MB) and `bvs` (~26.9 MB): ~40 MB.
  - Base binary, LLVM IR and mimalloc overhead: ~35 MB.
- **After `flush` / end of walk (`known.size() == 0`)**: `VmRSS` 394 MiB,
  `VmHWM` 394 MiB.
  - `flush` assembled each 32-bit element byte by byte with `t.concat`,
    interning 132,768 throwaway 16-bit and 24-bit constants plus 100,000 62-bit
    indices (`constants` grew to 708,026 / 18.6 MB), and built 100,000 `store`
    terms in `memo` (7.8 MB), `nodes` (7.8 MB), `made` (8.5 MB) and Bitwuzla's
    term arena (~90 MB).
  - `Terms`'s walk-only hash tables (`memo`, `nodes`, `bvs`, `made`: ~51 MB) and
    `Trace` (~120 MB RSS) remained live throughout `solver::decide()`.
- **After `solver::decide()`**: `VmRSS` 784 MiB, `VmHWM` 784 MiB.
  - Bitwuzla's search grew RSS by ~390–400 MB on top of the post-walk footprint
    (preprocessing, the 100,000-store array lemma for `x = 0`, 300,000 AIG ANDs,
    900,000 clauses in CaDiCaL).

## Changes

1. **Compact `Trace` (`src/walker/trace.h`, `walker.cpp`, `counterexample.cpp`,
   `witness.cpp`)**: intern `ir::SourceLoc` and step text strings in tables on
   `Trace`, store 32-bit indices in `Step` (shrinking `sizeof(Step)` from 152 B
   + 48 B heap to 48 B with zero per-step heap allocations), store steps in a
   `std::deque<Step>` to avoid 2x vector reallocation spikes, and record
   witness-only `Step::Branch` and `Step::Call` steps only when `opts.witness`
   is enabled.
2. **Release walk-only state before solving (`src/encode/terms.{h,cpp}`,
   `src/driver/main.cpp`)**: scope `Memory` and `Encoder` to the walk so they
   are destroyed before solving, and add `Terms::releaseWalkTables()` called
   after any `--dump-smt-*` output and right before `solver::decide()` to free
   `memo`, `nodes`, `bvs`, `made`, `implied`, `arith`, `filled`, `lambdas`,
   `under`, `reduced`, `selects`, `scratch`, `bounds`, `ranges`, and `freeOf`
   (~51 MB), keeping only `constants` and `bools` needed by `solverTerm()` and
   `Model`.
3. **Constant-element fast path in `Memory::flush()` (`src/memory/memory.cpp`)**:
   compute the shifted element index directly (`t.bv(APInt(ptrBits - shift,
   e.second >> shift))`) without constructing a 96-bit `ptr(e)`, and when all
   bytes of an element are known constants pack them directly into one `APInt`
   instead of interning intermediate 16/24/...-bit `t.concat` constants.
4. **Intrusive split `Leaf`/`Branch` `CellMap` (`src/memory/cellmap.h`)**:
   replace `std::shared_ptr<const Node>` (~128 B/node) with non-atomic intrusive
   refcounting (Vereda walks on one thread), store the distinguishing bit as a
   1-byte bit position, and split `Leaf` (40 B) and `Branch` (48 B), cutting
   `CellMap` from ~102 MB to ~45 MB.

## Incremental impact on `array_loop_summary_decrement_fail.c --no-summarize-array-loops`

Running `build/vereda` directly on the same prepared IR (`RelWithDebInfo`,
mimalloc):

| Build | Walk s | Walk peak MB | Solve s | Solve peak MB |
|---|---|---|---|---|
| Base (`0f60211a`) | 1.643 | 392 | 3.665 | 782 |
| + Compact `Trace` (1) | 1.299 | 292 | 3.473 | 688 |
| + Release walk tables (2) + `flush` fast path (3) | 1.155 | 288 | 3.407 | 648 |
| + Intrusive split `CellMap` (4) | 1.048 | 236 | 3.488 | 638 |

Walk time drops by 36%, walk peak by 156 MB (-40%), and solve peak by 144 MB
(-18%). The remaining ~563 MB above the ~75 MB of `Trace` (24 MB),
`Terms::constants` (16 MB), and base binary/LLVM (35 MB) is Bitwuzla's own term
arena and search state for the 100,000-store array chain.

## Benchmarks

Side by side (`build/vereda-base` at `0f60211a` vs `build/vereda`) on
`array_loop_summary*` with `--no-summarize-array-loops`:

| Test (`--no-summarize-array-loops`) | Walk s | Walk peak MB | Solve s | Peak MB | Verdict |
|---|---|---|---|---|---|
| `array_loop_summary` | 2.58 → 1.80 | 575 → 241 | 0.00 → 0.00 | 575 → 241 (-58%) | SUCCESSFUL |
| `array_loop_summary_fail` | 2.42 → 1.79 | 575 → 241 | 0.00 → 0.00 | 575 → 241 (-58%) | FAILED |
| `array_loop_summary_nondet` | 5.74 → 5.27 | 939 → 532 | 0.00 → 0.00 | 939 → 532 (-43%) | SUCCESSFUL |
| `array_loop_summary_counter_fail` | 1.61 → 1.12 | 383 → 246 | 0.61 → 0.57 | 424 → 271 (-36%) | FAILED |
| `array_loop_summary_decrement_fail` | 1.54 → 1.11 | 393 → 235 | 3.56 → 3.51 | 782 → 640 (-18%) | FAILED |
| `array_loop_summary_vla` | 0.03 → 0.03 | 42 → 42 | 0.01 → 0.01 | 42 → 42 | SUCCESSFUL |

`tests/bench.py` on `github_302`, `struct_array_ite_new_index_fail`,
`12_lms_new`, `13_ludcmp_new`, `github_123`, `26_exStbKey`,
`overflow_22_wordplay` and the `array_loop_summary*` tests with default flags
reported zero regressions (`github_302` peak 535 → 528 MB, `github_123` 53 → 48
MB). Full `ctest -j12` passed 100% (6,913 tests, 0 failures), and the
300,004-state counterexample output on `array_loop_summary_decrement_fail`
without summaries is byte-for-byte identical.

## Comparison against ESBMC on `array_loop_summary_decrement.c`

Measured on the passing test `tests/esbmc/array_loop_summary_decrement.c`
against ESBMC (`esbmc-ctype`, Bitwuzla 0.9.0) using `/usr/bin/time -f` (45 s
timeout for `--no-summarize-array-loops`):

| Tool and mode | Walk / Symex s | Solve s | Wall s | Peak MB | Verdict |
|---|---|---|---|---|---|
| ESBMC, default (summaries on) | 0.001 | 0.001 | 0.26 | 93 | SUCCESSFUL |
| Vereda base, default (summaries on) | 0.000 | 0.004 | 0.11 | 37 | SUCCESSFUL |
| Vereda head, default (summaries on) | 0.000 | 0.004 | 0.10 | 37 | SUCCESSFUL |
| ESBMC, `--no-summarize-array-loops` | 6.602 (+0.493 slice) | 0.001 | 7.91 | 306 | SUCCESSFUL |
| Vereda base, `--no-summarize-array-loops` | 1.491 | >45 (timeout) | 45.06 | 392 (walk) | TIMEOUT |
| Vereda head, `--no-summarize-array-loops` | 1.042 | >45 (timeout) | 45.06 | 236 (walk) | TIMEOUT |

With default loop summaries, Vereda uses 37 MB vs ESBMC's 93 MB. Without loop
summaries, Vereda's walk now takes 1.04 s and peaks at 236 MB (down from 392 MB,
and 70 MB below ESBMC's 306 MB symex peak), while Bitwuzla 0.11 still times out
proving UNSAT on the 100,000-store array chain (where `esbmc-ctype` simplifies
the formula before calling Bitwuzla).
