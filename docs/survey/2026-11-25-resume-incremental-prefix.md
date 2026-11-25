# Resuming incremental BMC inside an unconditional loop (#175)

Main at 71609b70 already includes #194's skipping of bounds below a known
trip count. This follow-up keeps the same sequence of bounds and reuses a
walk stopped inside a loop, rather than changing the schedule again.

## The checkpoint

At a loop's bound, the walker can retain its live frame when the loop is
top-level in the entry function, the reached guard folds to true, the
header is the frame's only pending arrival, and there are no returns.
There is then no completed exit state or caller continuation to invalidate
when adding iterations. Nested loops and calls within the completed prefix
are allowed; suspension itself must be outside them.

The checkpoint owns the frame's header arrivals, memory states, phi values
and path facts, alongside the existing encoder bindings, object table,
checks, trace, term builder, solver and CFG analyses. The next bound removes
the synthetic unwinding check and its trace event, then starts at that
header's saved iteration. Existing assertion claims keep their indices and
gain the new iterations' violations. If the loop exits, the remaining
function blocks run normally. A later unconditional outer loop can become
the next checkpoint.

This is a prefix of the larger-bound walk, not a speculative state after a
truncated loop. No solver constraints are popped: all retained constraints
belong to completed prefix instructions, and checks are queried as
assumptions. The old unwinding check is never asserted globally. Terms'
walk tables remain live only while the checkpoint needs them. Once no
checkpoint survives, walker, encoder and memory are freed before solving,
as before, and the next bound starts with a fresh solver and walk.

An exit under a symbolic guard, a pending return, a cut inside a nested
loop or recursion, or any other shape that cannot satisfy those conditions
uses the existing fresh-walk behavior. General continuation of merged exits
and call stacks remains future work; this change is part of #175.

## Measurements

LLVM 23.1.6, camada 0.25 / Bitwuzla 0.11.0, ILP32, incremental BMC and
`unreach-call.prp`. Main and branch use the same clang and library models.
Wall times include compilation and output. The machine also ran regression
tests during these measurements, so the exact ratios are indicative.

| Task | Main | Branch | Bounds, both |
|---|---|---|---|
| `array-examples/standard_vector_difference_ground` | TRUE, 15.13 s | TRUE, 7.22 s | 17, last 100001 |
| `loop-acceleration/array_2-1` | FALSE, 0.41 s | FALSE, 0.31 s | 12, last 2049 |
| `loop-acceleration/array_2-2` | TRUE, 0.31 s | TRUE, 0.21 s | 12, last 2049 |
| `array-lopstr16/partial_lesser_bound-1` | timeout, 60 s | timeout, 60 s | 2913 → 178302 |

The last task still needs a summary for its global counter and ten million
iterations. Main reached bound 5825; the branch reached 356603. It uses
more memory at the deadline (36 → 378 MB) because it has walked much
farther. Reducing only `SIZE` from ten million to 4096 makes both runs finish:
TRUE in 31.04 s on main, 0.31 s here. That case has no SCEV trip count and
keeps every bound in its original sequence, exposing the removed quadratic
prefix work. On `standard_vector_difference_ground`, peak memory is
883 → 881 MB; the branch resumes 16 times instead of rebuilding each prefix.

## ReachSafety

All 1407 unreach-call tasks from the Arrays, ControlFlow, Heap and Loops
list used in the earlier survey ran with a three-second wall limit, 6 GiB
address-space limit per process and 16 workers. This is a screening run,
not a replacement for the longer BenchExec decision gate; the existing #12
Actions run was left untouched. Base and head alternate their order per
task, use the same model and input files, and are checked against the
task's expected verdict.

| Run | Correct TRUE | Correct FALSE | UNKNOWN | Timeout | Wrong |
|---|---|---|---|---|---|
| Main | 419 | 287 | 2 | 699 | 0 |
| Branch | 424 | 287 | 2 | 694 | 0 |

163 tasks resume a prefix. No answered task is lost. The five additional
answers were rerun with a 60-second limit, alongside two previously
sensitive tasks:

| Task | Main | Branch |
|---|---|---|
| `loops-crafted-1/mono-crafted_14` | TRUE, 5.33 s | TRUE, 2.92 s |
| `loops-crafted-1/mono-crafted_3` | TRUE, 5.33 s | TRUE, 2.87 s |
| `loops-crafted-1/mono-crafted_9` | TRUE, 5.33 s | TRUE, 2.87 s |
| `loops-crafted-1/sum_natnum` | timeout, 60 s | TRUE, 1.77 s |
| `nla-digbench-scaling/lcm1_valuebound2` | TRUE, 2.42 s | TRUE, 2.82 s |
| `nla-digbench-scaling/egcd3-ll_valuebound10` | timeout, 60 s | timeout, 60 s |
| `memory-model/2SB` | FALSE, 3.93 s | FALSE, 3.93 s |

The `mono-crafted` tasks keep their 20 bounds and resume 19 times.
`sum_natnum` keeps all 20001 bounds, resumes 20000 times, and no longer
pays for a walk from the start at every bound. The last three tasks never
resume: their differences at the three-second limit reflect load and the
timeout boundary, not the new mechanism.

After simplifying the checkpoint's control flow, all 163 resuming tasks
were screened again: no wrong verdicts or lost answers. Two more finish
within three seconds under the different load, illustrating why the longer
reruns, rather than the short timeout boundary, are used for timing claims.
`sum_natnum` also proves TRUE again in 1.57 s.

## Regression coverage

The final full 7005-test run completes with 2828 passes, 4177 expected skips and
no failures. Five focused tests exercise multiple consecutive loops,
memory and calls through a volatile local, a late assertion added to an
existing claim, symbolic exits, nested-loop fallback, and a violation
witness whose nondet call occurs before a resumed prefix. Existing bound
selection and formula-dump tests pass without changing their expectations.
