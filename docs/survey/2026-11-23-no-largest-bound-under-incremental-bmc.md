# No largest bound under --incremental-bmc

Recorded 2026-11-23.

`--incremental-bmc` stopped at `--max-k-step` (50) with `no verdict up to
bound 50`. In the ReachSafety rerun on f53bf26d that ended 107 of the 159
tasks ESBMC answers and Vereda did not, after a median of 0.5 s of the 60 s
limit (#84). `--unlimited-k-steps` now lifts the bound, as ESBMC's does, and
`--k-step N` raises it by N each time (default 1); `vereda.py
--property-file`, the SV-COMP path, passes `--unlimited-k-steps --k-step 8`
unless the caller gives a bound or step of its own. The importer maps both
ESBMC flags.

On the 107 tasks, with no largest bound, 60 s and 6 GB each, 12 at a time:

| --k-step | correct | wrong | timeouts |
|---|---|---|---|
| 1 | 46 | 0 | 61 |
| 2 (ESBMC's wrapper) | 53 | 0 | 54 |
| 4 | 55 | 0 | 52 |
| 8 | 56 | 0 | 51 |
| 16 | 56 | 0 | 51 |

A larger step checks fewer bounds, and each bound is a new walk and a new
solve, so it reaches the deep bounds these tasks need sooner; past 8 nothing
more was gained. On the other 359 tasks of that run that stopped at bound
50 (ESBMC does not answer them either), step 8 answers 17 correctly, again
with no wrong verdict, and the rest run to the limit.

Each walk used to print one `Unwinding loop ... iteration N` line per loop
iteration, and with no largest bound every bound repeats them all: one task
wrote 2 GB in 60 s, and the first run of the 359 filled the disk. Under
`--incremental-bmc` the walks no longer print those lines; `Checking bound
k = N` still shows the progress.
