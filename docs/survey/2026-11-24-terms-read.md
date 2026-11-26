# Why Terms::read led the walk's profile

Recorded 2026-11-24.

A perf profile of the walk on array_loop_summary_decrement_fail with
`--no-summarize-array-loops` (100,000 iterations of `j -= 2; a[i] = j;`, then
a read of `a[x]` at a nondet x) had `Terms::read` on top, at about 12% of the
samples. `read` is the walk that `Terms::select` does for a constant index:
down a chain of stores at other constant indices, through joins whose sides
agree, for at most 256 nodes, to a stored value, a constant array's value, or
nothing, in which case `select` builds a fresh select below the stores it
passed.

## Calls, depth and results

Counted with counters in a scratch build of main (1290acd4) with #166
(7078c7c5, which takes 100,001 solver calls out of this walk), by the caller
of `select`:

| test, without summaries | caller | calls | repeated (array, index) | nodes walked | outcome |
|---|---|---|---|---|---|
| array_loop_summary_decrement_fail | `Memory::flush` | 100,000 | 0 | 25,567,104 | 99,743 fresh selects, 257 the zero fill |
| array_loop_summary | `Memory::flush` | 99,425 | 0 | 25,419,648 | 99,167 fresh selects, 258 the zero fill |
| array_loop_summary | `Memory::readOwn` | 397,696 | 298,272 | 101,678,592 | 396,672 fresh selects, 1,024 stored values |

No call met a join. `a` is an object of 32-bit elements (#107), zero-filled
as a global, and each `a[i] = j` at a constant address goes to the known
bytes; past 4,096 of them they all move to the element array. For each
element, `flush` read its old value before looking at which of its bytes
were known, and here all four always are: every one of the 100,000 reads,
almost all of them a 256-node walk ending in a fresh select term, was thrown
away. In array_loop_summary, the third loop's loads read each element at a
constant address byte by byte, so `readOwn` asked for the same select four
times, and each time walked the same 256 stores. On the first 21 of the
array_loop_summary tests without summaries, 82% of the `readOwn` calls
repeated an earlier one.

## The change

`flush` builds the old element only when one of its bytes is not known, and
`select` keeps what it returned per array and constant index. A select at a
constant index depends only on its two terms, as the folding state it reads
(`filled`, `lambdas`, `under`, the memo) never changes for a term once it
exists, so the cache gives the term the walk would build; terms live in the
solver's arena, so a key cannot come to name another term.

The select `flush` no longer builds has one side effect that goes with it:
over a loop summary, a fresh select of the summary's lambda l at index i
adds the constraint that l[i] is the summary's value at i. Without the
select nothing refers to l at i, so that constraint binds nothing else, and
a later read that reaches l at i adds it then. The new tests
summary_flush_whole and summary_flush_whole_fail reach this case: their
formulas lose that one assertion and keep their verdicts.

`--dump-smt-only` gives the same output with main+#166 and the branch+#166
on 3,636 of the 3,800 tests directly under tests/vereda and tests/esbmc (the
other 164 need a file, a witness or their own directory, or took over 60 s
under load), but for the temporary path in the three tests that dump the IR
and Z3's dump of github_3977_4, which numbers its `let` variables
differently as six unused terms are no longer built; with the variables
renamed in order, it is the same. A second comparison, over the 6,542
single-file tests of every suite under tests/, found the same, with four Z3
dumps renumbered. So did 40 of the 42 array_loop_summary tests run without
summaries, with `--no-summarize-array-loops` added; the other two,
stride_bound and stride_bound_fail, have no array and unwind a loop past
the 300 s limit.

After the change, the decrement test calls `read` from `flush` zero times,
and array_loop_summary walks 25,419,648 nodes in 99,424 `readOwn` reads, the
first read of each element, instead of 127 million in 497,121. Those 256-node
walks that find nothing are what is left: `read` is still 9% of that run.

## Time and memory

Release builds with the installed LLVM 21.1.2 and glibc's malloc (no
MIMALLOC_SOURCE_DIR in either), main (1290acd4) and the branch, both with
#166 applied, on an 80-core machine under a load of 20 to 37 from other
jobs. Three runs each, interleaved; walk time and the peak memory the walk
reports. The first round also has the `flush` change alone; the second is
the final code, against main again.

| test, without summaries | build | walk s | walk peak MB |
|---|---|---|---|
| array_loop_summary_decrement_fail | main+#166 | 5.29, 5.77, 5.58 | 465 |
| | `flush` change only | 3.90, 3.84, 3.85 | 421 |
| | branch | 4.06, 3.90, 4.22 | 421 |
| | main+#166, second round | 6.29, 6.01, 6.15 | 465 |
| | branch, second round | 3.82, 4.13, 4.05 | 421 |
| array_loop_summary (`--dump-smt-only`) | main+#166 | 18.64, 19.82, 18.05 | 1,104 |
| | `flush` change only | 16.07, 16.52, 16.92 | 1,076 |
| | branch | 12.97, 13.31, 13.50 | 1,084 |
| | main+#166, second round | 22.20, 18.49, 20.29 | 1,105 |
| | branch, second round | 13.77, 14.00, 14.00 | 1,083 |

On the decrement test the walk takes 3.8 to 4.2 s instead of 5.3 to 6.3 s
(ESBMC 8.7's symex of the same loop takes 6.4 s), the solve is unchanged
(5.4 to 6.2 s with both), and the whole run's peak resident memory falls
from 1,056 MB to 994 MB. On array_loop_summary the walk takes about 30%
less; the cache costs it 7 MB of the 28 MB the `flush` change saves.

`tests/bench.py --list` on 12_lms_new, 13_ludcmp_new, github_302,
github_1289 and every array_loop_summary test, each also without summaries
(a copy with `--no-summarize-array-loops` added), the two binaries side by
side, 180 s each. The rows that changed or ran long (walk, solve and wall
seconds, peak MB, verdict):

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| array_loop_summary, no summaries | 37.09 → 23.51 | - | - | 1281 → 1261 | TIMEOUT |
| array_loop_summary_fail, no summaries | 19.76 → 13.76 | - | - | 1295 → 1274 | TIMEOUT |
| array_loop_summary_nondet, no summaries | 50.48 → 29.00 | - | - | 2178 → 2151 | TIMEOUT |
| array_loop_summary_nondet_fail, no summaries | 32.29 → 20.44 | - | - | 1502 → 1455 | TIMEOUT |
| array_loop_summary_counter, no summaries | 5.89 → 4.15 | - | - | 552 → 521 | TIMEOUT |
| array_loop_summary_counter_fail, no summaries | 5.53 → 4.45 | 1.85 → 2.10 | 12.76 → 11.95 | 673 → 613 | FAILED |
| array_loop_summary_decrement, no summaries | 5.49 → 4.21 | - | 41.58 → 42.54 | 552 → 509 | KILLED |
| array_loop_summary_decrement_fail, no summaries | 5.63 → 3.96 | 5.72 → 5.54 | 17.06 → 15.04 | 1084 → 1023 | FAILED |
| array_loop_summary_stride, no summaries | 2.28 → 1.59 | - | 68.90 → 63.24 | 227 → 204 | KILLED |
| array_loop_summary_stride_fail, no summaries | 3.06 → 2.07 | - | 85.32 → 87.34 | 227 → 205 | KILLED |
| array_loop_summary_vla, no summaries | 1.13 → 0.82 | 1.59 → 1.25 | 2.98 → 2.30 | 76 → 76 | SUCCESSFUL |
| array_loop_summary_store_fail, no summaries | 5.32 → 5.54 | - → 59.60 | 62.19 → 73.87 | 599 → 3938 | KILLED → UNKNOWN |
| github_302 | 54.66 → 53.62 | 45.01 → 42.34 | 100.85 → 97.12 | 722 → 722 | FAILED |

The other rows, the tests with summaries among them, take under 0.3 s with
both. 12_lms_new, 13_ludcmp_new and github_1289 now walk in under 0.05 s on
main, so they no longer measure the walk. Both store_fail runs reach the
4 GB limit in the solver, with the same formula, so its two endings are the
limit's, not the change's. bench.py exits 1 because sixteen FUTURE tests
(k_induction, uninit, verifier_assert, witness and their _fail and no
summary copies) give no verdict with either binary, as they need a flag
Vereda lacks or do not compile; it counts that against the head.

A profile of the decrement test's walk with the branch has no sample in
`read` or `select`; the top is now building constants from strings
(`APInt::toString`, the `bvs` DenseMap, `mkBVFromBin`, `strtoull`).

## Not done

Each step of `read` looks its node up in `nodes` and the store's index up
in `values`, two hash lookups, and on array_loop_summary the 256-node walks
that find nothing are most of what is left. Marking a store at a constant
index in its node, and comparing index terms by pointer (constants are
shared), would save one lookup a step; a table of the stores below an array
would make a read one lookup, at the price of a table for each array read.
The decrement test needs neither. A read of a byte array (`readOwn` without
elements, `readArrays`) also builds the provenance select of every byte,
which only a load of a pointer or a copy uses; no measured test spends time
there.

Follow-up issues: #315.
