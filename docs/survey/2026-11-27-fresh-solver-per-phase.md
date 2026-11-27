# A fresh solver instance per solving phase

Recorded 2026-11-27.

Bitwuzla 0.11 runs normalization, skeleton preprocessing and full variable
substitution only on an instance's first `check_sat` (`apply_normalization =
d_num_preprocess == 1` in its `src/preprocess/preprocessor.cpp`), so every
later query on the same instance solves without them. Under `--incremental-bmc`
the walk resumes across bounds, and since the terms belonged to the solver,
one instance had to answer every bound: the trivial queries of the early
bounds left the real one without its preprocessing (#380, causes 1 and 2), and
the state of earlier queries stayed (#381, cause A). In the default mode the
walker's feasibility queries went to the instance that then decided the
checks, and the second phase got a fresh instance only by walking the program
again.

camada's new `SMTSolver::renew()` replaces the backend's instance by a fresh one
over the same term manager, so every term stays valid. Vereda now renews at
the start of each solving phase (under incremental BMC only with
`--fresh-solver`, see the decision below) (a bound's assertions, then its unwinding
assertions, and each further query of `--multi-property`), and asserts again
the constraints it asserted at top level. Those all go through
`Terms::addConstraint` (memory layout, object sizes and bases, lambda
reductions, the walker's assumptions), which now records them; the walker
never pushes or pops, and camada asserts its own facts again. A bound that is
too small renews once more before the walk resumes, so that the walk's
queries do not see the last phase's query. `--incremental-solver` keeps the
old behaviour: one instance with `bv-cnf-scope=permanent`, each check under
an assumption, across checks and bounds. Backends without renew (Z3, cvc5)
keep it too: a resumed bound solves incrementally and the default mode walks
again for its second query.

## Why the second walk existed

`decideNonIncremental` asserts each phase's disjunction instead of assuming
it, so a second query (the unwinding assertions after the assertions held, or
the checks left under `--multi-property`) needed a solver without it. Terms
could not move to another solver, so the program was walked again into a new
one, and the IR module had to stay alive until solving ended in case it was
needed. A renewed instance gives that query a clean solver over the same terms
and checks, so with Bitwuzla the walk is never repeated and the module is
freed before solving. This is sound because renew keeps every term, the
checks and the trace refer to those same terms, and the model a violation
reads is the renewed instance's own. `2026-11-26-perf-claims-remeasured.md`
(item 7) found that the second walk cost little time; what goes is the code
path and the module kept alive.

## Measurements

sv-benchmarks at `/home/mgadelha/tools/sv-benchmarks/c`, `-m32
--property-file unreach-call.prp`, 60 s CPU and 6 GB address space per task,
10 runs at a time on 40 cores (`taskset -c 0-39`), one run each. All binaries
link the same camada build (the `feat/renew` tree with Bitwuzla 0.11.0), so the
columns differ only in Vereda. main is `9e58fa66`; branch is this change;
`--incremental-solver` is the branch with that flag. CPU seconds and peak
resident memory in MB; TO is a timeout, OOM out of memory.

With `--incremental-bmc`:

| task | main | branch | branch `--incremental-solver` |
|---|---|---|---|
| array-crafted/mapsum3 | 36.3 s 5227 OOM | 1.5 s 184 | 36.4 s 5354 OOM |
| array-crafted/mapsum4 | 60.2 s 3286 TO | 17.4 s 1261 | 60.2 s 3416 TO |
| array-crafted/mapavg3 | 29.3 s 5343 OOM | 2.1 s 231 | 28.7 s 5330 OOM |
| nla-digbench-scaling/lcm1_valuebound5 | 31.6 s 704 | 5.0 s 149 | 31.9 s 704 |
| nla-digbench-scaling/egcd3-ll_valuebound10 | 60.0 s 698 TO | 35.7 s 341 | 60.0 s 699 TO |
| nla-digbench-scaling/egcd2-ll_valuebound5 | 12.3 s 265 | 6.3 s 133 | 12.3 s 269 |
| nla-digbench-scaling/dijkstra-u_valuebound100 | 4.9 s 218 | 3.6 s 131 | 4.8 s 218 |
| nla-digbench-scaling/lcm2_valuebound20 | 60.0 s 268 TO | 60.0 s 145 TO | 60.0 s 264 TO |
| nla-digbench-scaling/knuth_valuebound100 | 31.7 s 402 | 60.0 s 245 TO (94.3 s, 243 MB) | 31.8 s 412 |
| nla-digbench-scaling/geo3-ll_valuebound10 | 44.1 s 435 | 60.0 s 212 TO (360.6 s, 402 MB) | 50.3 s 524 |
| reducercommutativity/sum05-2 | 39.5 s 54 | 0.12 s 45 | 39.6 s 54 |
| reducercommutativity/sep60-2 | 23.6 s 60 | 0.13 s 45 | 24.2 s 63 |
| reducercommutativity/sep40-1 | 5.7 s 51 | 0.12 s 45 | 7.2 s 51 |
| reducercommutativity/avg05-1 | 60.0 s 66 TO | 0.12 s 45 | 60.0 s 76 TO |
| reducercommutativity/sum10-1 | 60.0 s 70 TO | 0.11 s 45 | 60.0 s 70 TO |
| reducercommutativity/sum20-2 | 60.0 s 70 TO | 0.11 s 45 | 60.0 s 72 TO |
| reducercommutativity/sum40-2 | 60.0 s 81 TO | 0.12 s 45 | 60.0 s 82 TO |
| reducercommutativity/sum60-2 | 60.0 s 107 TO | 0.12 s 45 | 60.0 s 99 TO |
| array-crafted/mapsum1 | 60.0 s 117 TO | 0.13 s 45 | 60.0 s 123 TO |

Every answer given is correct (all these tasks are TRUE). The figures in
brackets are the branch run again with a 600 s limit.

Without `--incremental-bmc` (the default mode):

| task | main | branch | branch `--incremental-solver` |
|---|---|---|---|
| nla-digbench-scaling/lcm1_valuebound5 | 3.8 s 205 | 2.5 s 126 | 4.3 s 203 |
| nla-digbench-scaling/knuth_valuebound100 | 54.0 s 462 | 57.2 s 364 | 60.0 s 556 TO |
| nla-digbench-scaling/dijkstra-u_valuebound100 | 9.4 s 363 | 3.5 s 131 | 11.0 s 369 |

The branch answers 10 tasks main does not (the three array-crafted tasks
that ran out of memory or time, egcd3-ll_valuebound10, and six of the nine
tasks of #380 cause 1; it answers the other three 50 to 330 times faster), is
1.4 to 6.3 times faster on the other nla-digbench tasks of #381, and uses less
memory on every task: 1.6 to 4.7 times less on the nla-digbench ones, 23 and
28 times less on mapavg3 and mapsum3. It loses on the two tasks the earlier measurement
predicted, where what one instance learns at a bound helps the next:
knuth_valuebound100 now needs 94 s instead of 32 s (3.0 times), and
geo3-ll_valuebound10 361 s instead of 44 s (8.2 times; part of that run
shared the cores with a regression suite, so take it as a rough figure), with
memory about the same. Default mode gains too, since the walker's
feasibility queries no longer stay on the instance that decides the checks;
knuth there is within noise (57 against 54 s) with a fifth less memory.
`--incremental-solver` matches main within noise on every task, as it should.

A variant that also dropped `bv-cnf-scope=permanent` under `--incremental-bmc`
(Bitwuzla's default, adaptive, for the walker's queries and the phases)
measured the same within 7 % on every task, so the solver is still created as
before.

## The skip-const patch

#380 proposed skipping the solver call for an unwinding assertion that folds
to true under `--incremental-bmc`, so that the first real query was the
instance's first. With a fresh instance per phase the nine tasks of cause 1
take 0.11-0.13 s, the time skip-const gave, so the patch is not applied. It
would still help `--incremental-solver`, which keeps those tasks timing out,
but that flag is meant to keep the shared instance exactly as before.

## BenchExec against the base

BenchExec on ReachSafety Arrays, ControlFlow, Heap and Loops, 60 s per task,
`--incremental-bmc`, this change against its base (main with #392, which
already skips the constant unwinding queries behind most of the
reducercommutativity gains above). The branch answers 11 tasks the base did
not: in nla-digbench-scaling lcm1_valuebound10, egcd3-ll_valuebound20, the
fermat tasks at unwindbound1 and 2, mannadiv_unwindbound100,
hard-ll_unwindbound10 and egcd2-ll_valuebound10, and loop-new/nested-1 and
theatreSquare. It loses 11 the base answered: loop-new/gauss_sum and its three
reducer variants (about 16 s on the base, timeouts here), loops/sum01-2 (6 s
to a timeout), geo3-ll_valuebound10, prod4br-ll_valuebound100,
fermat1-ll_valuebound20, freire1_valuebound10 and 50, and dll_of_dll-2.
ps3-ll's wrong FALSE on the base becomes a timeout. On the tasks both answer
the same, the branch uses 14 % more CPU time and 5 % less memory.

So across the bounds of incremental BMC, renewing the instance at every bound
is a trade-off, not a gain: what the kept instance learns at one bound helps
as many tasks as its missing preprocessing hurts. The default-mode numbers
above (a fixed bound, one walk) show no loss: dijkstra-u_valuebound100 9.4 s
to 3.5 s, lcm1_valuebound5 3.8 s to 2.5 s, knuth_valuebound100 54 s to 57 s,
within noise, with a fifth less memory, and the BenchExec run did not cover
that mode.

## Decision

Adopted, after the BenchExec run: under incremental BMC, a fresh instance per
solving phase of a resumed bound is opt-in, `--fresh-solver`; without it the
resumed bounds share one instance as before this change. With a fixed bound
(`--unwind`, or `--no-incremental-bmc`), each solving phase renews the
instance whenever the backend can (Bitwuzla), and the second phase renews
instead of walking the program again, which the measurements above show only
gaining. `--incremental-solver` keeps one shared instance everywhere, and
conflicts with `--fresh-solver`. Choosing per bound between a renewed and a
kept instance, by what the previous bound's solve cost, is the follow-up
(#387) that would make the fresh instance worth turning on by default.

In the same change incremental BMC became the default: without `--unwind`,
Vereda checks bounds 1, 2, ... up to `--max-k-step` (50) as `--incremental-bmc`
did, and `--no-incremental-bmc` gives the old default, unrolling each loop
until its next iteration is unreachable. `--no-unwinding-assertions` and
`--dump-smt-only` without `--unwind` keep the old default too, since
incremental BMC cannot pick a bound without checking it or solving; with an
explicit `--incremental-bmc` they are still errors. The k-step flags ask for
incremental BMC and are errors under `--no-incremental-bmc` or `--unwind`.
ESBMC's default checks one bound, so `tests/import_esbmc.py` adds
`--no-incremental-bmc` to an imported test whose ESBMC flags give neither
`--unwind` nor an incremental mode (`--incremental-bmc`, `--k-induction`,
`--falsification`, `--termination`, ...).

Left open: choosing per bound between a renewed and a kept instance (knuth,
geo3); skip-const for `--incremental-solver`; renew for Z3 and cvc5 in camada,
which would retire the second walk; and a regression test for the renew before
a walk resumes, which no test in the suite needs today.

Follow-up issues: #387, #388, #389, #390, #391.
