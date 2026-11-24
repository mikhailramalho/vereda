# Ranges from assumptions and branches

Recorded 2026-11-24.

Issue #120, second step; the first (#163) put a constant in place of a value
the path makes equal to it. Now a comparison with a value whose range the
path bounds settles as it is encoded: after `__VERIFIER_assume(n <= 10)`, or
on the else side of `if (n > 10) return`, `i < n` is false at `i == 10`, so
the loop's back edge folds away and the loop stops unwinding there, with no
unwinding assertion left; a branch on `n > 20` is pruned.

## Design

Where the facts live. A path's facts were a persistent map from a term to the
constant it equals (`Encoder::Facts`, memory's trie, copied per arrival as a
pointer, keyed by the term's id). The trie is now a template over its cell,
and a fact holds the constant if any and the term's bounds: its least and
greatest value as an unsigned and as a signed number (`Terms::Bounds`), for
bit-vectors of at most 64 bits. An equality gives both (`x = 5` is 5 to 5
either way).

What narrows. `Encoder::learn` takes every condition #163 took: an
assumption, an assumed `__VERIFIER_assert` (one that is no check under the
property), a branch or switch edge; and, new, `k < trips` inside a
summarised loop's iteration, which the iteration's guard holds.
`Terms::narrow` finds the literals of the condition as `equalities` does
(through `and` when it holds, `or` when it fails, `not`, a Bool `ite` with a
constant arm; both now walk one `literals` function), and for each `<` or
`<=`, unsigned or signed, as it holds or fails (`!(a < b)` is `b <= a`),
bounds each side by the other's range: `x < y` gives `x <= max(y) - 1` and
`y >= min(x) + 1`. The other side is usually a constant, but its range
counts, so `assume(n <= 10)` then `assume(i < n)` bounds i by 9. A
comparison that no value can satisfy (`x < 0` unsigned) says nothing, as the
path it is on has no execution. The two orders bound each other where an
interval does not cross the sign bit: signed `[1, 4]` is unsigned `[1, 4]`,
signed `[-6, -1]` is unsigned `[2^w - 6, 2^w - 1]`, signed `[MIN, 10]` says
nothing unsigned.

How they are used. Only `Terms::decide`, which the encoder calls on an
`icmp` it has just built when the path has facts, and which answers for that
path: the comparison's value bound to the instruction is the constant, and
the term itself is not changed. It uses #204's `Terms::range`, given a
callback (`Known`) that narrows every sub-term the range is computed through
to that term's bounds, so a fact on n settles `n + 3 < 7` and `(long)n < 5`;
signed comparisons use `srange`, the signed interval from the range when it
does not cross the sign bit, from the bounds, and through a sign extension.
A `=` is false when the two ranges, or the signed ones, are apart.

Scope and joins. Facts are per path exactly as #163's are: each arrival and
return carries its own; a join keeps, for each term every arrival has a fact
on, the hull of the bounds (and the constant if all agree), so
`if (c) assume(m < 3); else assume(m >= 0 && m < 5);` leaves `m <= 4`, and
a term one arrival knows nothing of loses its fact. A call continues with
the join of its returns; a summary leaves with the facts its arrivals
share, since the loop may not run.

## Why it is sound

A fact holds on every execution of its path: it comes from a condition that
is a conjunct of the path's guard at that point (an assumption, the edge's
own condition, the summarised iteration's `k < trips`), never from a counted
assertion, after which the path goes on where it fails
(`assume_range_join_fail`). A join's hull holds wherever any arrival's fact
does.

A fold by a fact holds only under its path's guard, so it must never reach a
term used elsewhere. Terms' builders never read facts: they fold, hash-cons
and cache (selects, lambda reductions, ranges, concat joins) exactly as
before. The ranges computed under a path's facts go to a map cleared per
query, never to `ranges`. The only folded value is the `icmp`'s, bound in
the encoder's environment, whose uses are dominated by the instruction or
reach it through a phi on an edge from it, so they run on executions of the
path that computed it. #204 recorded the trap of the other kind: a bound
registered before the guard holding it was built folded the guard's own
comparison away and gave FALSE on a TRUE program. Here a guard is never
built with facts in force: the edge's guard, the assumption's conjunction
and the summary's `body` exist before the condition is learned, and none of
them is built by `decide`.

A summarised iteration's terms are later put at each element's index n
(`substitute`), and are read only where the store's guard, with n for k,
holds, and n is below the trip count: the facts the iteration learned hold
there with n for k, as #204's bound on k does.

`Terms::order` now settles signed comparisons by `srange` for every caller,
not only under a path's facts: before, it gave up as soon as a range reached
the sign bit; now two ranges that are both negative, or a sign extension of
a known range, settle too. Each of those is a sound range of the term
everywhere. A build that counted them found none in the 1,196 ReachSafety
tasks whose walk at `--unwind 10` ended within 30 s.

The tests in `tests/vereda`: `assume_range_branch` (every check folds in
the dumped formula: signed and unsigned in both directions, the else side of
a branch, a conjunction, `u + 5` and a widening, `=`, a join of two narrowed
paths, `assume_abort_if_not` in a callee), `assume_range_limits` (the
greatest unsigned of 8, 32 and 64 bits, signed values near both ends, a
negative int read as unsigned, a negative range through a sign extension),
`assume_range_loop` (four loops bounded by an
assumed or branched-on input stop at their range, no unwinding assertion
left), and the `_fail` versions of each with the failure at the edge of the
range, so a bound one off or a fact that leaked past a join would lose it:
`assume_range_loop_fail`, `assume_range_branch_fail` (11 failures),
`assume_range_limits_fail` (a wrap past the greatest unsigned, a signed bound
read as unsigned, `n - 1` for `n >= 1`), `assume_range_join_fail`,
`assume_range_unwind_fail` (`n <= 10` with `--unwind 5` still fails the
unwinding assertion), `assume_range_summary_fail`.

## Measurements

Base is main at ebca2f0c (#197), head this branch at 22009d62, both Release
with LLVM 21.1.2.

A loop bounded by an assumption, `assume(n <= 100)` and `i < n` with a
nondet body, under `--unwind 1000`: main unrolls 999 iterations and leaves
the unwinding assertion to the solver (2 checks, 8.7 s); this branch stops
at 100 (1 check, 1.1 s). With `n <= 40`, 6.6 s against 0.21 s. Under
`--incremental-bmc` the two take the same time: the bounds go 1, 3, 7, ...,
and the bound that passes the range ends both.

Regression tests: the vereda suite and the 674 tests `ctest -R
'esbmc.*(loop|assume|array|summary|unwind)'` selects pass. Both binaries ran
them with `REGRESS_STATS` (882 tests record numbers): wall 191.0 s against
191.1 s, walk 1.9 s against 2.1 s, solve 38.2 s against 37.6 s, no verdict
changes but the new tests', and `tests/bench.py --stats` found no test past
its margins to measure again.

ReachSafety. A build that counted the folds walked all 1,407 unreach-call
tasks at `--unwind 10` (30 s each): 1,196 walks ended, 868 learned a range,
and in 327 a range decided a comparison. Those 327 walked at `--unwind 30`
with both binaries, without solving: 86 walk fewer loop iterations (7,337
against 1,825 in all; `nla-digbench-scaling/lcm1_valuebound1` 1,769 against
87, `cohendiv-ll_valuebound1` 899 against 2, the `geo*_valuebound1` tasks
29 against 0, where `0 <= k <= 1` and `k >= 1` make the loop's `c < k` false
at once), 211 keep fewer checks (705 against 428), none walks more of
either, and the walks take 3.93 s against 3.60 s.

Then the harness of the ReachSafety reruns (`vereda.py --incremental-bmc
--property-file unreach-call.prp`, 60 s and 6 GB in a systemd scope), both
binaries on each task at once, three tasks at a time, on 558 tasks: the 327
above and the other tasks with `__VERIFIER_assume` or `assume_abort_if_not`
that an older main answered.

| tasks | right, main | right, this branch | lost | wrong | both right: time |
|---|---|---|---|---|---|
| 327 where a range decides | 180 | 181 | 0 | 0 | 277.3 s against 278.4 s |
| all 558 | 411 | 412 | 0 | 0 | 918.0 s against 917.7 s |

The one gained, `nla-digbench-scaling/knuth_valuebound50`, took 58.5 s
against a timeout, which is the time limit, not the change. The largest
moves either way are solver time on formulas that differ a little
(`egcd2-ll_valuebound5` 13.8 s to 8.7 s, `geo1-ll_valuebound10` 14.2 s to
18.6 s); of the 146 tasks both time out on, this branch reached a higher
bound in 26 and a lower one in 41. The ranges cut iterations mostly where
the tasks were already answered at a small bound: under `--incremental-bmc`
a loop stops early only once the bound passes the range, and the solver,
not the walk, takes the time in the tasks that time out.

## esbmc-unix2/23_picosat-846_01

The pull request's bench flagged this test (KNOWNBUG, recorded as taking
over 300 s): UNKNOWN in 0.46 s on main, killed for memory after 38 s with
this branch. Main stops on `llvm.ptrmask`, which the printf model uses to
align a `va_arg`, in the option parsing it walks for `argc >= 2`. Under
`--unwind 2` the paths with `argc >= 2` end at main's first loop, so
`1 < argc` in `picosat_main` is false, as the ranges now show: the walk
skips the parsing and ends (3.4 s, 301 MB, 9 checks), and Bitwuzla runs out
of memory on the sixth check, the unwinding assertion of that first loop,
which needs no more than `argc >= 2`, after five writes-to-const checks in
picosat's allocator. With `--no-pointer-check` the walk takes 134 MB and
that check still runs past 150 s. Nothing of the ranges grows: with
`__VERIFIER_assume(argc == 1)` added, which main's constants already fold,
both binaries walk the same formula (7 checks, 274 MB, a byte-identical
10.2 MB dump) and both run out of 4 GB, at 108.6 s and 108.8 s. Main's quick
UNKNOWN is the walk stopping on a path that no execution takes;
`assume_range_dead_unsupported` has that shape. Why the solver needs
gigabytes for that check is another issue.

## Not done

The memory stage's checks (an index against an object's size) are built by
Terms without the path's facts, so `assume(i < 10); a[i]` still leaves the
bounds check to the solver; deciding them needs the facts passed to
`memory` and `checks`. A switch's cases are compared by the walker, not
decided. `x != c` could move an end of x's range; `x = y` between two
non-constant terms, and a fact on `zext(x)` narrowing x, are not learned.
