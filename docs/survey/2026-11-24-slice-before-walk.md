# Slicing the rolled IR before the walk: not worth building yet

Recorded 2026-11-24.

Issue #115 asks for a static slice of the rolled IR before the walk, dropping
instructions whose results reach no check, no branch condition and no memory
a later read may see, and to measure first what it would save on the
walk-heavy tests. It would save under 1% of their walk time, so the pass was
not built.

The measurement was a scratch build (Release, LLVM 21, not committed) that
timed every solver call the walker makes, timed the encoding of each walked
non-call instruction, and marked each instruction live or dead with a
whole-program liveness: roots are branch and switch conditions, calls to
bodyless functions other than pure intrinsics, allocas and atomics; a
defined callee's parameter is live when a live instruction uses it, a ret
operand when some call's result is live; a store is live when a live load or
bodyless call may read an object it may write, with objects from
`getUnderlyingObjects`, arguments followed to their call sites, and anything
else read or written as every object. That is the slice the pass would have
computed. The tests are the six with the longest walk in the CI stats of
main at 8db85e5d; the array_loop_summary tests come next, and #115 already
rules them out, since every store feeds the read the check makes.

| test | walk | solver calls in the walk | encoding, live | encoding, dead | walked / dead instructions |
|---|---|---|---|---|---|
| esbmc/12_lms_new (--overflow-check) | 98.6 s | 97.9 s (605 calls) | 0.03 s | 0.54 s | 9,232 / 1,735 |
| esbmc/13_ludcmp_new | 65.0 s | 64.6 s (172) | 0.29 s | 0.07 s | 2,188 / 186 |
| esbmc/github_302 | 50.7 s | 50.6 s (19) | 0.06 s | 0 | 427 / 0 |
| esbmc/github_1289 (--overflow-check) | 9.1 s | 9.0 s (99) | 0.05 s | 0 | 2,413 / 0 |
| esbmc/github_306 | 5.8 s | 5.8 s (75) | 0.05 s | 0 | 604 / 0 |
| esbmc/github_1344 | 2.3 s | 2.1 s (22,100) | 0.02 s | 0 | 46,301 / 0 |

On 12_lms_new a fifth of the walked instructions are dead (all of `lms()`'s
filter arithmetic and the values of `d[]`, which only `lms()` reads), but
building their terms takes 0.54 s of a 98.6 s walk. Everything the walker
does besides calling the solver takes under 1% of the walk on every test
but github_1344 (8%, spread over 22,100 small solver calls), so no slice can
save more than that. The dead instructions never reach the solver: the walk
builds one hash-consed term graph and the solver sees only the cones of the
loop guards it is asked about, which are branch conditions and so live.

The walk time is the solver call per loop iteration that, without
`--unwind`, asks whether the next iteration is reachable (walker.cpp,
`loop`). On 12_lms_new and 13_ludcmp_new those guards carry floating-point
comparisons of values computed from constants (the conditions of `sin()`'s
series loop and `gaussian()`'s rejection loop; the pivot test
`fabs(a[i][i]) <= eps` that can leave `ludcmp()`'s loops early). The encoder builds
floating-point operations with the solver's `mkFP*` calls directly, past the
folding in `encode/terms`, so a guard the program computes from constants
alone still goes to Bitwuzla, and each one is deeper than the last. Folding
floating-point operations on constants (APFloat under round-to-nearest-even)
would settle these guards without the solver, which is where the walk time
of these two tests is; that is a separate change from #115.

Revisit the slice if a test shows up whose walk is spent building terms
rather than in the solver, or once checks for memory safety give loads and
stores checks of their own (the slice must then keep every access a check
needs).
