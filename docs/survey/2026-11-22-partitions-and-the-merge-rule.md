# Partitions and the merge rule

Recorded 2026-11-22.

Partitions first. Counting flushes of known bytes on the spike tasks: an
access at a pointer with an unknown object flushes every known byte, but
that happens only in uthash (6 times, 2,500 bytes at k = 2); Problem08 and
SpamAssassin never flush. A select that also skips stores into another
known object, (obj, i) against (obj', j), which is what one array per
object would give, changed nothing on the spike tasks or the 189-task
check. Known bytes already keep most objects apart, so a points-to
analysis waits until a task needs it.

The merge rule instead: at a join, an object whose two sides hold different
values that multiply, divide or take a remainder goes to the arrays on both
sides, so Bitwuzla bit-blasts that arithmetic only where a read needs it.
Problem08 and Problem102 have none, so they keep their values.

| Task | k | Walk before | Walk after | Solve before | Solve after | Peak after |
|---|---|---|---|---|---|---|
| Problem14 | 1 | 2.1 s | 3.0 s | out of memory | 341 s | 4.2 GB |
| Problem14 | 2 | | 12.7 s | | out of memory | 15 GB |
| uthash | 2 | 36 s | 21 s | 18 s | 8.4 s | 0.6 GB |
| Problem08 | 3 | 5.4 s | 6.1 s | 30 s | 29 s | 1.8 GB |
| Problem102 | 3 | 5.2 s | 5.8 s | 29 s | 30 s | 1.7 GB |
| SpamAssassin | 12 | 0.03 s | 0.03 s | 15 s | 14 s | 0.2 GB |

Problem14 at k = 1 is back, faster than before folding (569 s). The check
is a walk over the value's terms that stops at array reads, memoized; a
first version capped its depth and called a capped term arithmetic, which
the memo then spread to every term above it and moved Problem08's
constant-valued globals too (walk 10 s, solve 73 s at k = 3). The 189-task
check gives the same verdicts and time.
