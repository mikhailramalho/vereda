# Array loop summaries

Recorded 2026-11-23.

The first slice of issue #36, after ESBMC's array loop summaries
(esbmc/esbmc#10563): a counting loop whose body writes only `a[i]` is
replaced by its effect instead of being unrolled. isl, reductions,
read-modify-write of other elements and nested loops are not part of it.

## Design

`analysis/loops.h` decides the shape on the IR, with LLVM's SCEV. The
walker, at the loop's header, evaluates the SCEVs it needs to terms (the
trip count, the header's phis in iteration k and after the last one, the
store's first address), then walks one iteration at a fresh index
k < trips, with the header's phis bound to their values in iteration k and
each nondet call returning element k of a fresh array. That iteration
reaches every check of the body, as ESBMC's copied iteration does. Its
memory is the memory before the loop, which is exact because no access in
the body reads what another iteration writes. The walker keeps the store's
guard and value, both terms in k, drops the back edge and the iteration's
own exit, and leaves the loop with the header's values after the last
iteration and the summary in memory.

camada has no lambda term, so the summary is a lambda array in the term
builder: `Terms::lambda(sort, name, def)` makes a fresh array whose element
at i is `def(i)`. A read of it reduces to `def(i)` as it is built, as
ESBMC's simplifier reduces its lambda; a constant index folds completely.
A read through stores or choices above it stays a select, and the
definition at that index becomes a constraint. That is sound and complete
because the solver sees an array only through selects (there is no array
equality), so it observes the lambda only at the indices it is read at.
`Terms::substitute` gives the guard and value at another index: it
rebuilds the term with k replaced and folds again, remembering per symbol
the subterms that do not contain it, so the path condition under the loop
is walked once.

`Memory::summarise` turns the written object's arrays into lambdas over the
arrays before (its own, or the shared ones tested by object id): for
element j at offset o, n = (o - first) / stride and b = (o - first) mod
stride; it is written when n < trips, b < size and the guard holds at n,
with that byte of the value at n. An object of word elements (#118) is
read and written only as whole values, so its element is the value. The
object's known bytes go to the arrays first.

## Shapes summarised

A loop is summarised when it is innermost, has a preheader and a latch
(loop-simplify), and exits normally only at its header, after the number of
iterations `getExitCount` gives. The header has no store or call, since it
runs once more than the body. Each header phi is an affine AddRec of the
loop: counters, `j += 3` or `j -= 2` before or after the write, steps
other than 1, pointer induction variables. The body may branch (a
conditional write), compute integers, read globals and allocas from before
the loop other than the written one, and read its own element at the
store's address. It may call `__VERIFIER_nondet_*`, `__VERIFIER_assert`,
`assert`, the error functions, `abort` right after one, pure intrinsics,
sanitizer traps, and defined functions (four deep, with no loop, which
might not end) that themselves only do these and neither store nor, when
the loop stores, load. Each nondet call gets an array of its own, as two
calls can share a location. A path end in the
body must report a violation under the property checked: under
`--property unreach-call` an assertion is an assumption, and an assumption
that stops some iterations is something a summary cannot express. Any
other exit of the loop is a block ending in `unreachable` after such a
violation. The store, if any, is the only one, of an integer, at
`{first,+,stride}` with a constant stride at least its size, into a global
or an alloca from before the loop, and trips times stride fits in the
pointer width. Floats are left out (the solver's float terms are opaque to
substitution), and so is undef (one fresh value would stand for all
iterations). Nothing is summarised in a program that calls
`pthread_create`.

Not summarised yet: loops exiting at the latch (do-while, rotated), a
negative stride (`for (k = n; k > 0; k--) a[k - 1] = ...`), two stores, a
read of another element (`a[i] = a[i - 1] + 1`), a defined `reach_error`
before `abort` under the default property, nested loops.

A summarised loop has no unwinding assertion: with `--unwind N` below its
trip count, unrolling would fail at the bound and the summary does not, as
in ESBMC.

## Flags

Every loop analysis/loops.h accepts is summarised by default, also those
that only check or count, which ESBMC summarises only under
`--summarize-array-loops` (decided 2026-11-24); Vereda has no such flag (the
importer drops it), and `--no-summarize-array-loops` summarises none.
Vereda's own tests of unwinding (loop_const, loop_unwind_false,
multi_property*, incremental_bmc_false and _unlimited) use such counting
loops and pass `--no-summarize-array-loops` to keep testing unwinding. An
imported test that bounds such a loop, github_5188_bounded (CORE, now
FUTURE), now finds the bug past the bound, which ESBMC's default unrolling
cuts off, and records FAILED. unwind_truncation_warning and
multi_property_incremental_final_verdict_unknown did too until the rule
below unrolled their short loops again. The FUTURE tests that needed
`--summarize-array-loops` run. With `--witness`,
loops that call nondet are unrolled: a witness needs a waypoint per nondet
value, and the summarised iteration gives one, for index k only (ESBMC
drops all waypoints instead).

A loop with no store whose trip count is a constant of at most 256 when the
walker reaches it is unrolled instead. Unrolled, its reads are at constant
indices into bytes the walker knows, and they fold; summarised, it leaves
the solver a read at a nondet index, which costs seconds when the loop runs
many times. The first bench of the store-free default found that in five
imported tests, measured here with `tests/run.py` (60 s limit, a machine
at load 45) for each threshold:

| Test | 0 | 8 | 16 | 32 | 64 | 256 | 1024 |
|---|---|---|---|---|---|---|---|
| esbmc-cstd/intrinsic_memset_array | 39.7 | 13.9 | 2.15 | 0.27 | 0.29 | 0.30 | 0.30 |
| esbmc-cstd/intrinsic_memset_array_union | 39.2 | 13.4 | 1.75 | 0.28 | 0.29 | 0.25 | 0.31 |
| esbmc-cstd/intrinsic_memset_array_struct_malloc | 53.0 | 34.5 | 16.3 | 0.45 | 0.38 | 0.41 | 0.42 |
| esbmc-cstd/intrinsic_memset_array_struct | timeout | timeout | timeout | timeout | 0.96 | 0.94 | 0.88 |
| esbmc-field-sensitivity/repro7521_w2048_fail | 6.40 | 5.30 | 0.32 | 0.31 | 0.31 | 0.32 | 0.33 |

The memset tests check up to `sizeof(struct) * array_size` bytes in a loop
called for every prefix; repro7521 checks 10 times in a loop called 2000
times. None of the tests that gain from summarising a loop with no store
(the array_loop_summary tests that needed `--summarize-array-loops`,
summary_checks and summary_checks_fail) changes up to 1024: their counts
are 100000 or symbolic. Unrolling costs about 0.1 ms an iteration (a
check loop over a written array: 0.21 s summarised against 0.31 s
unrolled at 1024 iterations, 1.6 s at 16384), so 256, four times the
smallest threshold that fixes all five, costs at most some 25 ms a loop.
A loop with a store keeps its summary at any count: unrolling store loops
of up to 16, 64 or 256 iterations changed none of these tests, nor the
array_loop_summary or summary tests, and made update and no_slice slower
at 256.

## Numbers

f7b40b00 (main, with #118 and #119) against the same plus this change,
side by side with `tests/bench.py --list`, 100 s per run, on an 80-core
machine shared with other jobs. Main runs the tests that pass
`--summarize-array-loops` without it.

| Test | Walk s | Solve s | Wall s | Peak MB | Verdict |
|---|---|---|---|---|---|
| decrement_fail | 8.15 → 0.00 | 5.80 → 0.00 | 20.34 → 0.19 | 1079 → 80 | FAILED |
| decrement | 8.30 → 0.00 | - → 0.01 | timeout → 0.19 | 557 → 80 | → SUCCESSFUL |
| counter_fail | 10.01 → 0.00 | 1.23 → 0.00 | 17.52 → 0.19 | 670 → 80 | FAILED |
| counter | 8.58 → 0.00 | - → 0.00 | timeout → 0.15 | 558 → 79 | → SUCCESSFUL |
| store | 10.64 → 0.00 | - → 0.09 | timeout → 0.34 | 608 → 81 | → SUCCESSFUL |
| store_fail | 9.59 → 0.00 | - → 0.02 | timeout → 0.21 | 607 → 81 | → FAILED |
| stride | 3.59 → 0.00 | - → 0.01 | timeout → 0.22 | 254 → 78 | → SUCCESSFUL |
| stride_fail | 3.37 → 0.00 | - → 0.00 | timeout → 0.26 | 254 → 80 | → FAILED |
| array_loop_summary | 23.32 → 0.00 | - → 0.00 | timeout → 0.17 | 1216 → 78 | → SUCCESSFUL |
| array_loop_summary_fail | 27.21 → 0.00 | 0.00 → 0.00 | timeout → 0.20 | 1201 → 79 | → FAILED |
| nondet | 42.93 → 0.00 | - → 0.00 | timeout → 0.28 | 2120 → 80 | → SUCCESSFUL |
| nondet_fail | 26.16 → 0.00 | - → 0.01 | timeout → 0.23 | 1477 → 79 | → FAILED |
| stride_bound | timeout | | timeout → 0.21 | → 78 | → SUCCESSFUL |
| stride_bound_fail | timeout | | timeout → 0.26 | → 79 | → FAILED |
| vla | 0.76 → 0.00 | 1.04 → 0.01 | 2.07 → 0.18 | 120 → 80 | SUCCESSFUL |
| update | 0.02 → 0.01 | 0.16 → 0.01 | 0.36 → 0.21 | 86 → 79 | SUCCESSFUL |

The decrement test's formula keeps no array: the read of `a[x]` reduces
to the stored value at x, or the zero below it. The other tests of the
family give the same verdict on both and stay under 0.4 s. Of the 42
array_loop_summary tests, the 6 KNOWNBUG ones (out of memory or time)
become CORE, and so do 15 FUTURE ones that only lacked the flag (the two
neighbour tests among them, whose loop is left to unrolling). 13 stay
FUTURE for what Vereda lacks: bounds checks, threads, `--k-induction`,
`--uninitialised-vars-check`, YAML witnesses, a goto into a loop, and a
`__VERIFIER_assert` the imported source both uses undeclared and defines.

Locally the 200 imported tests that have a `for` loop writing `a[i]`, and
Vereda's own suite, pass; CI runs the rest.

## Seen on the way

A zeroed global under `Memory::MinSeparate` (1024 bytes) is a chain of one
store per byte. `unsigned char a[1000]` read at a symbolic index after a
loop takes Bitwuzla 51 s on main, summary or not; at 3000 bytes the object
is one constant array and the same test takes 0.2 s.

Follow-up issues: #297.
