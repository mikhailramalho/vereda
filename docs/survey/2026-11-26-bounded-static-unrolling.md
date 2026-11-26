# Bounded loop unrolling after SCEV summarization (#253)

## Motivation

On ReachSafety Loops, ESBMC (`-s incr --goto-unwind --unlimited-goto-unwind`)
verifies `loop-lit/afnp2014` (`while (y < 1000 && __VERIFIER_nondet_int())`) in
0.47 s at `k = 1`, whereas Vereda with `--incremental-bmc --k-step 2
--unlimited-k-steps` walked and solved 501 incremental bounds (`k = 1, 3, ...,
1001`) in 57.02 s (115 MB peak RSS).

## Root cause in LLVM IR

At `-O0`, clang compiles `while (y < 1000 && __VERIFIER_nondet_int())` through
a `land.end` merge block holding `phi i1 [ false, %while.cond ], [ %tobool,
%land.rhs ]` and `br i1 %0`. Because `while.cond` does not exit the loop
directly, LLVM's `ScalarEvolution` reports `Unpredictable` even for
`getConstantMaxBackedgeTakenCount`, and LLVM's `loop-unroll` cannot unroll the
loop.

Threading trivial `i1`-phi merge blocks (`threadBoolPhis` in `src/ir/load.cpp`:
redirecting constant-boolean incoming edges around blocks that contain only
`phi i1` + `br i1 phi`, without running full `simplifycfg`) immediately exposes
`constant max backedge-taken count is i32 1000` to SCEV even after
`stripUBAssumptions` strips `nsw`/`nuw`/`exact`, while leaving overflowing
signed counters (`for (int i = 0; i < 2147483647; i += 2)`) and wrapping
unsigned loops (`for (unsigned i = 0; i != 3; i += 2)`) `Unpredictable`.

## Evaluation: static LLVM `UnrollLoop` vs. walker-side `maxBackedges`

We prototyped and benchmarked both designs after `threadBoolPhis`, both capped
at `maxTrips <= 1024` and `trips * insts <= 16384`, and both skipping loops
summarized by `analysis::Loops`:

- **Design A (static LLVM `UnrollLoop` + `instcombine` + `stripUB`):** unrolls
  eligible loops in LLVM IR before walking. Disabled when `--witness` is set so
  duplicated loop blocks do not disturb witness waypoints.
- **Design B (walker-side `maxBackedges`):** keeps rolled IR and lets
  `Walk::loop` unroll non-summarized loops whose SCEV constant max
  backedge-taken count (and nested subloop product) fits within the budget
  (`<= 1024` trips, `<= 16384` instructions, no calls to defined functions) in
  one pass, also skipping redundant per-iteration `unreachable()` queries when
  no `--unwind` is set.

Controlled measurements on LLVM 23.1.6 with `--incremental-bmc --k-step 2
--unlimited-k-steps`:

| Benchmark | Baseline (`main`) | Design A (static `UnrollLoop`) | Design B (walker `maxBackedges`) |
|---|---:|---:|---:|
| `afnp2014` (`y < 1000 && nondet()`, SAFE) | 57.02 s (501 bounds) | 0.20 s (1 bound, 43 MB) | 0.30 s (1 bound, 67 MB) |
| `afnp2014_bug` (FAIL at iter 15) | 0.20 s (9 bounds) | 0.30 s (1 bound, 65 MB) | 0.30 s (1 bound, 64 MB) |
| `nested_bounded` (`30 x 20 && nondet()`, SAFE) | >15 s timeout | 4.40 s (1 bound, 135 MB) | 4.31 s (1 bound, 133 MB) |
| `overflow_counter` (`i < INT_MAX; i += 2`, FAIL) | 0.20 s (6 bounds) | 0.20 s (6 bounds) | 0.20 s (6 bounds) |
| `array_loop_summary_decrement_fail` (100k iters) | 0.002 s (summarised) | 0.002 s (summarised) | 0.002 s (summarised) |

Without `--unwind`, Design B also drops `afnp2014` from 6.62 s (1,000 mid-walk
`unreachable()` solver calls) to 0.30 s.

## Decision

Adopt **`threadBoolPhis` + SCEV `getConstantMaxBackedgeTakenCount` in
`Walk::loop`** rather than static LLVM IR unrolling:

- In `--incremental-bmc`, set `unwindsAt = maxBackedges + 1` from
  `getConstantMaxBackedgeTakenCount` even when the loop has symbolic early
  exits (`!t.isTrue(reached)`). This extends #175's exponential bound doubling
  (`1 -> 3 -> 7 -> 15 -> 31 -> 63 -> 127 -> 255 -> 511 -> 1001`, 10 bounds
  instead of 501) to bounded loops with symbolic exits, dropping `afnp2014`
  from 57.02 s to **0.49 s** (69 MB peak RSS, 116x faster, matching ESBMC's
  0.47 s) while preserving early-bug detection at small bounds and all existing
  `--incremental-bmc` bound-progression tests.
- Without `--unwind`, skip redundant per-iteration `unreachable()` solver
  queries while `i <= maxBackedges` (within the 1024-trip / 16384-instruction
  budget and no defined-callee calls), dropping `afnp2014` without `--unwind`
  from 6.62 s (6.35 s walk) to **0.25 s** (0.020 s walk).
- Preserves rolled IR, block names and debug locations, so SV-COMP runs with
  `--witness` get the full speedup without disabling unrolling.
- Respects explicit `--unwind N` (`maxBoundedUnroll = 0`) and `--max-k-step`,
  and never interferes with `analysis::Loops` summaries.
