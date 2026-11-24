# ReachSafety rerun on a453547a

Recorded 2026-11-24.

The 1,407 unreach-call tasks of Arrays, ControlFlow, Heap and Loops again,
on main at a453547a, with the harness of the 2026-11-23 runs
(`~/tools/baselines/2026-11-23-reachsafety`): 60 s and 6 GB per task, each
in its own systemd scope under `setsid`, 12 at a time on the 80-core
machine, `vereda.py --incremental-bmc --property-file unreach-call.prp`.
Since f53bf26d that command also passes `--unlimited-k-steps --k-step 2`
(#93, #132) and `-ffp-contract=off` (#141). The binary is a Release build
with `-DDOWNLOAD_DEPENDENCIES=ON`, so the installed LLVM 21.1.2, mimalloc and
lld; the front end's clang is `/usr/lib/llvm-21/bin/clang`, since the
pinned release the earlier runs used is gone. ESBMC was not rerun: its
numbers are the 2026-11-23 run of ESBMC 8.3.0 (b12e1ead72) through
`esbmc-wrapper.py -s incr`. Another agent ran its own benchmarks on the
machine at the same time; the load average stayed between 15 and 29. The
run took 56 minutes. The per-task results are not in the repository.

The harness labels a run the kernel's OOM killer stopped as ERROR, because
`/usr/bin/time` never sees the killed binary: its peak memory is the front
end's, as it is for every run that times out. The journal's `oom-kill`
result for those two tasks' scopes made them MEMOUT below.

## Verdicts

Score as SV-COMP counts it: +2 a correct TRUE, +1 a correct FALSE, -32 a
wrong TRUE, -16 a wrong FALSE.

| Set | Run | Correct TRUE | Correct FALSE | Wrong TRUE | Wrong FALSE | UNKNOWN | TIMEOUT | MEMOUT | ERROR | Score |
|---|---|---|---|---|---|---|---|---|---|---|
| Arrays (440) | a453547a | 74 | 88 | 0 | 0 | 0 | 276 | 2 | 0 | 236 |
| | f53bf26d | 24 | 75 | 0 | 0 | 233 | 108 | 0 | 0 | 123 |
| | first run | 19 | 75 | 0 | 1 | 244 | 95 | 0 | 6 | 97 |
| | ESBMC | 41 | 87 | 0 | 0 | 0 | 303 | 9 | 0 | 169 |
| ControlFlow (73) | a453547a | 18 | 22 | 0 | 0 | 0 | 33 | 0 | 0 | 58 |
| | f53bf26d | 4 | 4 | 0 | 0 | 58 | 7 | 0 | 0 | 12 |
| | first run | 12 | 16 | 6 | 8 | 25 | 6 | 0 | 0 | -280 |
| | ESBMC | 17 | 23 | 0 | 0 | 0 | 33 | 0 | 0 | 57 |
| Heap (136) | a453547a | 72 | 50 | 0 | 0 | 0 | 14 | 0 | 0 | 194 |
| | f53bf26d | 72 | 47 | 0 | 0 | 4 | 13 | 0 | 0 | 191 |
| | first run | 71 | 47 | 0 | 9 | 4 | 5 | 0 | 0 | 45 |
| | ESBMC | 71 | 47 | 0 | 0 | 0 | 18 | 0 | 0 | 189 |
| Loops (758) | a453547a | 293 | 143 | 0 | 0 | 5 | 317 | 0 | 0 | 729 |
| | f53bf26d | 222 | 121 | 0 | 0 | 216 | 199 | 0 | 0 | 565 |
| | first run | 223 | 121 | 0 | 0 | 218 | 196 | 0 | 0 | 567 |
| | ESBMC | 282 | 136 | 0 | 0 | 0 | 334 | 6 | 0 | 700 |
| All (1407) | a453547a | 457 | 303 | 0 | 0 | 5 | 640 | 2 | 0 | 1217 |
| | f53bf26d | 322 | 247 | 0 | 0 | 511 | 327 | 0 | 0 | 891 |
| | first run | 325 | 259 | 6 | 18 | 491 | 302 | 0 | 6 | 429 |
| | ESBMC | 411 | 293 | 0 | 0 | 0 | 688 | 15 | 0 | 1115 |

Vereda now answers 760 tasks correctly against ESBMC's 704, with no wrong
verdict; ESBMC has none either. Without a largest bound, nearly every task
Vereda does not answer runs to the time limit: the 5 UNKNOWNs are
reducer-made tasks with irreducible control flow, and the 2 MEMOUTs are
`array-multidimensional/copy-2-u` and `transpose-u`, which reach 6 GB
past bound 999 (ESBMC times out on both).

Vereda alone answers 69 tasks (ESBMC times out on 64 and runs out of memory
on 5), ESBMC alone 13. Vereda times out on all 13. Six stopped at bound 50
on f53bf26d: four now pass bound 1,800 (`loop-acceleration/array_2-1` and
`array_2-2`, `array-lopstr16/partial_lesser_bound-1`,
`array-examples/standard_vector_difference_ground`), each bound walked
again from the start and solved in a few milliseconds, and
`loop-invgen/seq-3` and `up` reach bounds 89 and 175 with solves of 2 to
3 s. The other 7 timed out on f53bf26d too.

## Gained and lost since f53bf26d

192 tasks gained, 1 lost. To split the gains, the 192 ran again with
`--no-summarize-array-loops`, same setup:

| Cause | Tasks |
|---|---|
| Loop summaries (#124): time out without them | 80 |
| No largest bound with a step of 2 (#93, #132) | 76 |
| setjmp and longjmp lowered to control flow (#103) | 32 |
| A bodyless call writes what its arguments point to (#111) | 2 |
| Calls to variadic functions walked (#122) | 1 |
| calloc of symbolic size (#147) | 1 |

The summaries' 80 are 42 `array-examples`, 8 `loop-acceleration`, 8
`loop-zilu`, 6 `loop-lit` and 16 others. Of the 76 the bound explains, 67
stopped at bound 50 on f53bf26d and 9 timed out there. Those 9 are
nla-digbench-scaling tasks whose even bounds were the slow ones: four
`_unwindbound` FALSE tasks never finished bound 2 and now find the bug at
bound 3 in under 2.2 s, and five `_valuebound` TRUE tasks now answer in 20
to 58 s by skipping solves such as `freire1_valuebound5`'s 50 s at bound
2. The 2 of #111 call `__VERIFIER_nondet_memory`, whose void result had
given `unsupported: type void`; the variadic one is `ldv-regression/
fo_test` (`open`), the calloc one `loops/linear_search`. The longjmp gain
matches what `2026-11-23-setjmp-and-longjmp.md` measured. No change in
verdict traces to #141, #143 (no task calls `pthread_create`), #148, #152,
#158 or #159.

The lost task is `ntdrivers/floppy.i.cil-3`, TRUE in 29 s on f53bf26d and
now a timeout (ESBMC times out too). Its bound 1 alone, bisected with
Release builds against the installed LLVM: d956a686 solves it in 2 s,
a0a466f6 (#82, a stored pointer with no object converted at its load)
runs past 30 s, and the walk grows from 457 to 615 symbols. The sprintf
model (#148) is not the cause: with `sprintf` renamed to a function
without a body, bound 1 still takes over 60 s on a453547a. #82 also slows
`floppy.i.cil-1`'s bound 1 from 3.2 s to 20 s; it still answers, in 24 s.
#154, which landed after this run and gives pointers from outside the
program memory of their own, undoes both: on 1290acd4 bound 1 takes 2.2 s
again (its parent, c31cac6d, still runs past 30 s), and on a52160a3
floppy.i.cil-3 gives TRUE at bound 17 in 18 s and floppy.i.cil-1 FALSE in
3.5 s.

On the 568 tasks both Vereda runs answer, a453547a takes 691 s against
1,043 s; most of the gain is on nla-digbench-scaling tasks, where the step
of 2 skips slow bounds. Slower by more than 2 s:
`list-ext2-properties/list_and_tree_cnstr-1` (1.2 to 28 s) and
`ldv-sets/test_mutex_unbounded-2` (8.5 to 39 s), which bound 2 answers and
the step of 2 checks at 3 instead, as `2026-11-24-k-step-2.md` foresaw;
`floppy.i.cil-1` (#82); and four tasks slower by 2 to 5 s.

## Time and memory against ESBMC

On the 691 tasks both tools answer correctly:

| | Vereda | ESBMC |
|---|---|---|
| Total time | 1,045 s | 1,677 s |
| Median time | 0.21 s | 0.50 s |
| Faster on | 660 | 31 |
| Total of peak memory | 70.3 GB | 90.2 GB |
| Median peak memory | 90 MB | 121 MB |
| Largest peak memory | 631 MB | 579 MB |
| Less memory on | 654 | 37 |

By ESBMC's time: on the 514 tasks it answers in under 1 s, Vereda takes
132 s against 255 s; on the 135 between 1 and 10 s, 385 s against 469 s;
on the 42 over 10 s, 528 s against 953 s. Vereda is slower by more than
10 s on seven: `loop-lit/afnp2014` (47 s against 1.3 s), `nla-digbench-
scaling/lcm1_valuebound5` (32 against 5.8), `memory-model/2SB` (24 against
4.2), `loop-zilu/benchmark20_conjunctive` (19 against 8.7) and three more
nla-digbench-scaling tasks (`divbin2_unwindbound20`,
`bresenham-ll_valuebound20`, `prodbin-ll_valuebound50`). Most of
the 37 where ESBMC uses less memory are nla-digbench-scaling; the widest
gaps are `lcm1_valuebound5` (631 against 216 MB), `2SB` (493 against 251)
and `list-ext2-properties/simple_search_value-2` (302 against 122).

Vereda's median rose from 74 MB on f53bf26d to 90 MB, and the walk is not
why. On a small task the peak is that of the front end's clang or of the
binary's start. The installed clang, with its shared LLVM libraries, peaks
at 90 MB on `loops/array-1`; the earlier runs' timeouts, where only the
front end is measured, show about 59 MB for the pinned one. The release
binary, which `DOWNLOAD_DEPENDENCIES` links with lld, starts at 79 MB
resident (`vereda --version`), while the same commit linked by ld.bfd
starts at 34 MB and peaks at 75 MB on floppy.i.cil-3's bound 1 against 121
MB.

## Against the milestone 1 gate (#12)

On these four sets the gate's conditions hold: no wrong verdicts, and
Vereda is faster on 660 of the 691 shared answers and smaller on 654,
with 38% less total time. Still missing: the gate's large tasks
(esbmc/esbmc#9785's altera task, the list-model blow-ups, the large
drivers) are not in this set and have not been measured; the numbers must
be rerun under BenchExec (`contrib/benchexec/vereda.py`, which #161's
workflow now runs on GitHub's runners) to be published, with ESBMC run
alongside rather than taken from another day's load; and
the lld-linked binary's 45 MB of extra resident memory at start makes the
memory margin on small tasks smaller than it need be.
