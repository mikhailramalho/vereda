# Picosat first unwinding assertion: unrelated globals and later calls

Recorded 2026-11-24. Partial investigation of #214.

The first unwinding assertion in `esbmc-unix2/23_picosat-846_01` needs an
`argc` that reaches its third test and two non-null `argv` entries. On #209
at 9a6c822e, `--unwind 2 --no-pointer-check` walks the full three-file
program in 2.44 s, then Bitwuzla 0.11 runs out of a 1.5 GiB address-space
limit after 9.13 s total. The dump of all five checks is 2,555,471 bytes:
595 `base#` declarations, 616 textual `select bases` occurrences, and
7,138 textual `store` occurrences. These are syntax counts, not counts of
unique terms or solver statistics.

## Cutting the program down

All runs use the same Release #209 binary, clang 21, `--unwind 2` and
`--no-pointer-check`. The modified sources are experiments, not equivalent
replacements: only their first loop is unchanged. The last column gives the
verifier solver time and reported process peak, or the total time limit.

| Program | Result | Solver / limit |
|---|---|---|
| `main.c`, with `picosat_main` replaced by `res = 0`, linked alone | FAILED at `main.unwind.0` | 0.071 s, 56 MB |
| Same edited `main.c`, linked with `app.c` and `picosat.c` | timeout | 25 s total, 1.5 GiB address-space cap |
| Same edited `main.c`, plus 100 unrelated defined integer globals | FAILED at `main.unwind.0` | 0.211 s, 102 MB |
| Same, plus 300 globals | FAILED at `main.unwind.0` | 1.347 s, 174 MB |
| Same, plus 600 globals | timeout | 15 s total, 1 GiB address-space cap |
| Same 600 globals, also remove the two signal-handler calls | FAILED at `main.unwind.0` | 5.365 s, 368 MB |
| Same 600 globals, remove signal-handler calls and replace the later `strcmp` by a nondet integer | FAILED at `main.unwind.0` | 1.667 s, 250 MB |
| Stop immediately after the first loop, plus 600 globals | FAILED at `main.unwind.0` | 1.645 s, 250 MB |

Globals were defined in a separate C translation unit as
`int independent_global_N = N;`; none is referenced by the edited main.
For the timeout rows, the Python subprocess sets `RLIMIT_AS` on the compiler
and verifier process tree and imposes the wall timeout. Successful peak
figures are the verifier report, not the compiler's peak. A timeout means no
answer in that budget, not an observed memory exhaustion.

The edited main linked with the two picosat units still produces a 1,278,452
byte dump, with 586 `base#` declarations, 590 `select bases` and 7,138
`store` occurrences. Thus running the body of `picosat_main`, its heap
allocations, and its printf calls is not necessary for a slow solve. Removing
const-object checks is not enough either. The synthetic variants show that
unrelated globals amplify the cost, and that the later `strcmp` and
signal-handler calls add cost in combination with them. They do not separately
establish which solver operation or global constraint dominates.

To reproduce, copy `main.c`, replace `res = picosat_main (argc, argv);`
with `res = 0;`, and compile it through `vereda.py` with the original header
directory on `-I`. Add a separate source of `N` definitions of the form
above. In the signal-free version replace `setsighandlers ();` and
`resetsighandlers ();` by `(void) 0;`; in the nondet version also replace
`!strcmp (argv[verbose], "-v")` by a declared `__VERIFIER_nondet_int()`
call. For the first-loop-only version truncate the function after the first
loop and return zero. These substitutions retain the initial assumption and
the first loop verbatim.

## Interpretation and limits

Memory allocates global objects and asserts their layout before the walk;
later byte reads and bodyless calls may construct terms involving that
state. Per-check slicing could remove work irrelevant to the first check,
and #116's proposed layout constraints only for objects whose addresses
can become integers is a candidate. Neither is justified solely by these
timings: a sound change must preserve aliasing, address comparisons,
external-call effects, and constraints on object layouts. Measure the
actual slice or encoding before adopting it.

For comparison, local ESBMC 8.3 with Bitwuzla 0.9 solved the first-loop-only
cut in 0.037 s solver time and 0.53 s wall, reporting the same unwinding
violation. On the full three-file input it returned FAILED in 4.07 s, but
its default selected the later `base2flt` unwinding assertion, not the
first loop. A `--multi-property` full run timed out at 25 s under the same
1.5 GiB cap. This is not a comparison of the full input's same selected
check or of identical solver versions.

Not yet measured: Bitwuzla phase statistics and a same-check ESBMC formula
comparison. The available installation provides the static solver library
but no Bitwuzla command-line tool. These experiments narrow the source of
the cost but do not complete #214.
