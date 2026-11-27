# Termination from SCEV trip counts (#38)

Recorded 2026-11-27.

## What it does

`--property termination` (set by `vereda.py --property-file` for
`CHECK( init(main()), LTL(F end) )`) answers `TRUE` or `UNKNOWN`, never a
FALSE (non-termination is #39). Two ways lead to `TRUE`:

1. Without a walk (`src/analysis/termination.cpp`). From the entry, follow the
   direct calls. If no function recurses and, in every function reached,
   `getSymbolicMaxBackedgeTakenCount` is computable for every loop, every run
   ends. The answer takes a fraction of a second, whatever the bounds are,
   nondet ones included.
2. With the walk. Under the property no assertion counts, so a failed
   assertion ends its path as the abort it is, and the only checks left are
   the unwinding and recursion checks. When none of them can fail (or the
   walk without `--unwind` ends), no run takes more iterations or calls than
   the walk unrolled, so every run ends. It covers loops SCEV cannot count but which end soon: a
   search through a constant array, bounded recursion, a list of fixed length.

Some code runs out of the walk's sight: a function whose address is taken and
that a bodyless function may call (`atexit`, `qsort`, `signal`,
`pthread_create`), a constructor or destructor (also one listed by hand in
`.init_array` or `.fini_array`), an indirect call or inline
assembly that ir did not lower. When any of these is reachable, the answer is
`UNKNOWN` at once, without a walk.

## Why it is sound

What `end` means comes from sv-benchmarks' `properties/termination.md`: every
finite run ends (`exit`, `abort`, the return of `main`); calls to environment
functions that may block forever do not violate the property.

For the static proof:

- SCEV runs on the IR the walker walks. ir strips the poison-generating flags
  (`nsw`, `nuw`, `exact`, `inbounds`, `nneg`, `disjoint`, `samesign`), the
  `range`, `nonnull`, `noundef` and `dereferenceable` attributes and metadata,
  and rejects `mustprogress` (and clang runs with `-fno-finite-loops`). So
  SCEV cannot infer no-wrap from undefined behaviour or assume that a loop
  ends; its counts hold in wrap-around arithmetic, which is how the walker
  encodes the program. `for (int i = 0; i < n; i += 2)` is therefore
  unbounded (with `n == INT_MAX` it wraps forever), and so is
  `for (int i = 0; i <= n; i++)`; both are `UNKNOWN`
  (`tests/vereda/termination_wrap_unknown.c`). Tasks whose termination rests
  on the absence of signed overflow are lost; that is the price.
- A `__builtin_assume` (`llvm.assume`) would let SCEV take its condition as
  true, which C leaves undefined when it is false, so one in a reached
  function rules the static proof out. `__VERIFIER_assume` is an ordinary
  call to SCEV, so a bound known only through it is not used
  (`termination_assume_unknown.c`).
- The symbolic maximum is the minimum of the exit counts of the exits that
  dominate the latch (`computeExitLimit` gives up on the others), each
  evaluated when the loop is entered; any SCEV is a finite number, so each
  entry of the loop runs finitely many iterations. An earlier exit, a call
  that ends the run (`exit`, `abort`, `longjmp`, which ir lowers to a
  dispatch loop SCEV does not count) or one that blocks only shortens the run.
  `while (1)` with a nondet `break` has no computable exit and is `UNKNOWN`.
- Every cycle of a function is a natural loop: ir runs FixIrreducible, and the
  analysis checks `containsIrreducibleCFG` again. Every call goes to a
  function whose loops are checked in turn, or to a bodyless one, which ends
  the run, blocks, or returns without calling the program back; ir lowers the
  indirect calls (also those of the library models) to direct ones. With no
  recursion, the calls nest to a finite depth, so a run is a finite tree of
  finitely many iterations.
- Loops and calls in functions the entry does not reach do not matter.

For the walk:

- The walker used to end a path at an endless loop (`while (1);`) as at
  `unreachable`; under termination that path is the violation, so the loop is
  unrolled and the bound's check fails (`termination_endless_unknown.c`).
- A bodyless `reach_error` ended the path, as the error it is under the other
  properties; under termination it is an unknown function that returns
  (`termination_reach_error_unknown.c`). A failed `assert`,
  `__VERIFIER_assert` or `__assert_fail` ends the run, as in C.
- `--no-unwinding-assertions` would assume the bound instead of checking it,
  so the binary rejects it with `--property termination`.
- The array loop summaries replace a loop by its effect after SCEV's exact
  count on the same flag-free IR, so they end too, and the walk's other uses
  of SCEV (the bounded unrolls, the bounds skipped under `--incremental-bmc`)
  only spare solver calls.

Both ways assume, as all of Vereda does, that ir's passes keep the program's
meaning and that the walk over-approximates what it cannot model (an unknown
function returns any value and may write what it gets a pointer to). Memory
errors are out of the argument only in so far as they change the walk's
values; SCEV counts in registers, which no stray store reaches, and a loop
bound loaded from memory counts as an unknown, finite value.

## Measurements

The 395 tasks of `sv-benchmarks/c` whose `.yml` lists `termination.prp` (344
expected true, 51 false; none of the `termination-*` directories is in this
checkout), run as SV-COMP runs Vereda (`--incremental-bmc`, so unlimited
bounds with a step of 2), 30 s each, 4 GB:

| Expected | Static `TRUE` | Walk `TRUE` | Time limit |
|---|---|---|---|
| true (344) | 272 | 24 | 48 |
| false (51) | 0 | 0 | 51 |

No task expected false became `TRUE`; run again with 120 s each, 50 of them
reached the time limit and `loop-invgen/SpamAssassin-loop` ran out of the
4 GB. Every one of them stopped at a loop SCEV
cannot bound, and the walk raised the bound until the time limit. The static
proof takes about 0.2 s a task.

Of the 48 true tasks left, 29 are `array-memsafety`'s string and array
loops, whose exit depends on memory (`while (s[i] != 0)`, `while (a[i] > 0)
a[i]--`), and most others need a guard SCEV does not use, such as
`if (n == INT_MAX) return; for (i = 1; i <= n; i++)` (`loops/sum01_bug02`).

A first run treated `malloc` and `free` as bodyless functions that may call a
function whose address is taken, which made 6 `ldv-memsafety` tasks with
function pointers `UNKNOWN`; the walker models the allocator, so they are now
known to call nothing.

## Not done

- Counting loops whose exit reads memory: promoting the location to a
  register first (LICM's scalar promotion on a copy of the function), or the
  recurrent-state check of plan §7.4 step 2.
- Using `__VERIFIER_assume` conditions as SCEV facts: turning
  `__VERIFIER_assume(c)` into `llvm.assume(c)` on a copy would be sound here,
  since a failed assumption blocks the run, and would answer
  `termination_assume_unknown.c`.
- Bounding `i <= n` by a guard `n != INT_MAX`: SCEV turns `<=` into `<` only
  when `n`'s range excludes the maximum, not from a dominating condition.
- Mixing the two ways: a loop SCEV bounds could be summarised or skipped by
  the walk, so that only the loops it cannot count need unwinding.
- Non-termination (`FALSE_TERMINATION`), #39.
