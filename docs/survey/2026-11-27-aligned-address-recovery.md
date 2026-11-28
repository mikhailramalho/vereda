# Use alignment to simplify integer address recovery

Recorded 2026-11-27.

Issue #423 reports an ILP32 regression in `heap-manipulation/merge_sort-1`
after #395. Its original description said `-m64`; the reporter confirmed that
the measurements actually used `-m32`, matching the task's ILP32 data model.
The fixture is `merge_sort-1.i` from sv-benchmarks `5092e58a00`, SHA-256
`c8a604b7e04d7cdf87f612ffd9e01960547fa82d5aba8c47ab67cfeac1d9d0de`.

## Investigation

The walk takes less than 0.1 s; solving dominates. Restoring unconditional
symbolic-address splitting recovers ILP32 speed but builds more object scans
and raises peak memory. This would undo part of #395's benefit, and the same
change makes LP64 substantially slower, exceeding 300 CPU seconds against
184.7 s on current main. It is not a suitable general fix.

Initial LP64 experiments restoring the older `concat` joining, increasing the
entailment traversal budget, restoring whole-object havoc splitting, disabling
the direct-base shortcut, or disabling constant-array memoization did not
change this task's SMT formula. Path-local folding of object existence and
pointer provenance also left the formula unchanged. These experiments do not
justify removing #395's memory limits. Replacing modular membership with two
full-width interval comparisons did not finish within 300 s either.

## Change and correctness

`Memory::fromAddress` normally tests whether an exposed object holds address
`a` with `(a - base) <= size`, then constructs its object/offset pointer.
For an object smaller than its power-of-two alignment, this range, including
one past its end, fits in a single aligned block. With alignment 16 and size 8,
the same test is:

```text
a[W-1:4] == base[W-1:4] && a[3:0] <= 8
```

The layout constraints already require the base's low four bits to be zero
where the object exists. The test remains guarded by that object's existence.
It accepts offsets 0 through 8, rejects addresses below the base and in the
rest of the block, and cannot wrap into the next block. Size zero accepts
only the base. Objects whose size reaches their alignment retain the original
predicate because their one-past address can occupy the next block. An
experimental extension using an extra equality for that next-block address
exceeded 300 s on LP64, so it is not adopted.

Apply this rewrite only when the original range test did not fold. Preserve
path-bound reasoning, object IDs, offsets, null handling, the direct-base
shortcut, and every traversal/cache limit from #395. The offset still uses
subtraction; the optimization changes the membership predicate, not pointer
arithmetic. On the ILP32 bound-3 SMT dump, `bvule` occurrences fall from 882
to 147; `bvsub` occurrences remain 1,029. The LP64 task's SMT script is
byte-for-byte unchanged.

## Measurements and validation

Matched Release builds use LLVM 23.1.6, camada 0.25, mimalloc 3.7.0 and the
same Xeon E5-2698 v4 host. The issue used RelWithDebInfo; the local comparison
uses Release on both sides. Run the fixture with `-m32 --incremental-bmc
--property-file unreach-call.prp`, pinned to individual cores. CPU time
includes user and system time; memory is `/usr/bin/time` peak RSS in MiB.

The initial comparison measured 45.2 s / 791 MiB at `672fa26f`, 58.1 s /
557 MiB at `16dad640`, 60.6 s / 531 MiB on main `2ad10cd8`, and 44.8 s /
575 MiB with this change. Every run returned `FALSE_REACH`.

Two further pairs, with the base and head swapping cores for the last pair,
give these three-run results against main:

| Build | CPU seconds (three runs) | Median CPU | Median peak RSS |
| --- | --- | --- | --- |
| Main `2ad10cd8` | 60.59, 54.45, 54.01 | 54.45 s | 557 MiB |
| This change | 44.82, 45.38, 46.14 | 45.38 s | 585 MiB |

The median improves by 16.7%, returning to the pre-#395 time while retaining
most of its memory reduction. The original 32-split address recovery was
45.84 s / 837 MiB in a separate run, so restoring it would spend much more
memory for essentially the same speed.

The new `vereda/memory_addresses` unit test asks the solver to prove the full
recovered pointer equals the original modular specification for every address
and conditional allocation outcome, across 32/64-bit pointers, five alignments,
zero-size objects, and sizes below, at, and above the alignment. The own suite
has 714 passes and one pre-existing skip, including the symbolic-address regressions.

The #395 memory cases still complete with `-m32 --unwind 11
--no-unwinding-assertions --property unreach-call`: `ddv_machzwd_inb` uses
10.7 CPU seconds / 772 MiB and `loops/n.c24` 4.4 CPU seconds / 801 MiB,
both returning `TRUE`. These retain the earlier fix's memory behavior.

This addresses the small-object membership cost, not the general heap byte-array
solver cost or the higher-bound memory growth already tracked separately.

Follow-up issues: #381, #401
