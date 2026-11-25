# First reads through constant store chains

Revisit #183 on main 394cbd6e, after #249. The issue's original 100,000-element
array-loop benchmarks no longer exercise `Terms::read`: #203 raised the
known-byte limit to 1 MiB, so their 400 KiB arrays stay known. With summaries
disabled, the three-loop array example walks in about 1.4 seconds, has no
sampled time in read/select, and dumps a 59-byte false query. Temporary read
counters confirm zero calls on array_loop_summary, decrement_fail,
array_loop_summary_nondet and array_loop_summary_store_fail.

The other part of the original profile is also outdated: since constants
became tagged Terms in #197, reading a store's constant index needs no hash
lookup. The remaining lookup is finding each array term's Node in `nodes`.

## A workload that still exercises the issue

Scale the array to 270,000 unsigned elements, just past 1 MiB, fill it with
`3*i`, then read every element at a constant index. The known map flushes,
and the reads traverse its store chain. The initial probe checks every
read and stops before solving with `--dump-smt-only`:

| counter | value |
|---|---:|
| read calls | 262,145 |
| nodes/stores visited | 67,076,480 |
| stored-value hits | 256 |
| searches exhausting the budget | 261,889 |

An uninstrumented perf cpu-clock profile attributes 14.75% of samples to
Terms::read. The probe walks in about 13.6 seconds and peaks at 1,448 MB.
The trial parent-link optimization visits exactly the same nodes and keeps
the same hits and exhausted searches.

The permanent regression, mem_store_chain_reads.c, consumes the reads via
a volatile sink and checks its final value. This exercises the walk while
keeping the final query small. Asserting every array value instead leaves
a large array proof, obscuring the walk measurement. The new fixture runs
with `--no-summarize-array-loops` and is benchmarkable against main using
the head-fixture support from #252.

## Cached links without additional node storage

Node's x field is unused for Store operations. Use it to cache the parent
array's entry index plus one for constant-index stores. A read looks up its
first node normally, then follows these links through consecutive stores.
The entries live in a pointer-stable deque, and the nodes map never replaces
an existing entry. No node/entry size, new table, or solver representation
is introduced.

Zero means the parent had no recorded node. Keep the dynamic lookup in that
case: a backend rewrite may later make a formerly opaque array acquire a
recorded node. A negative result cannot be cached permanently. The metadata
is excluded from hash-consing keys and comparisons, so recording a parent
later cannot change store interning. Cache links only after a memo miss.

The 256-node budget, symbolic-index barriers, joins, filled-array defaults
and select caching are unchanged. The optimization changes how the same
nodes are found, not how far the search proceeds or which expressions it
returns. In particular, it must retain the boundary between the last
stored value that folds and the first read left as a select.

A complete index-to-store map per persistent array could avoid traversal,
but adds storage and maintenance costs to writes and branches. The measured
lookup cost can be reduced with the existing node field, so a broader index
is not introduced here. Increasing the read budget would change the folding
work and resulting terms; the budget remains unchanged.

## Validation

The store_reads C++ test checks long chains, hash-consing and repeated
selects, exact folding at and beyond the search limit, overwrites, old array
versions, opaque/filled tails, symbolic stores, joins and a parent affected
by a later backend rewrite. Symbolic equalities are checked with Bitwuzla.
The end-to-end stress test also passes under the normal regression limits.

Measurements use LLVM 23.1.6, camada 0.25/Bitwuzla 0.11.0, Release builds
and glibc allocation for both binaries. Temporary profiling counters are
not part of the change.

The full suite has 7,025 registered executions: 2,848 pass, 4,177 skip,
and none fail (215.20 seconds locally with eight concurrent executions).
The final C++ boundary assertions also pass independently.

Compare SMT dumps from 6,619 C/IR configurations across the suite, using
the same front end and fixtures with both binaries. All 3,211 configurations
that produce formulas on both sides have byte-identical dumps after replacing
their temporary directory paths with a common marker. The other 3,408 produce
no formula on either side (unsupported inputs/options or resource limits);
there are no one-sided dumps. Each comparison has a 10-second, 4 GiB limit.
Builtin-dependent and source-working-directory fixtures are omitted from
this dump comparison; the normal full suite still runs its registered tests.

## Final timings

Three interleaved runs per binary, alternating order, with summaries disabled
and `--dump-smt-only` to isolate the walk. Median walk times:

| case | main s | parent links s |
|---|---:|---:|
| mem_store_chain_reads | 11.714 | 9.912 |
| array_loop_summary | 1.417 | 1.409 |
| array_loop_summary_decrement_fail | 0.875 | 0.865 |

The stress case improves by about 15%, with peak memory 1,196–1,198 MB for
both binaries. Smaller arrays that stay in the known map remain unchanged.
These are local measurements, not a general speedup guarantee.

Comparing the full-suite statistics against the main-equivalent #249 suite
selects only the new stress fixture. The side-by-side bench.py run uses
head's new fixture with both binaries and reports no regressions: walk
12.77 to 10.13 seconds, wall 13.40 to 10.72 seconds, peak 1,197 to 1,196 MB,
and SUCCESSFUL on both sides. The solve is below the reported precision.
