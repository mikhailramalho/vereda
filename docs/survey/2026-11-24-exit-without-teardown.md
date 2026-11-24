# Exiting without tearing down the solver and the terms

Recorded 2026-11-24.

Once it had printed the verdict, the binary returned from `bmc()` and
`main()`, which destroyed the solver, the term builder, the memory model, the
checks and the trace, then the module and the LLVM context. After a large walk
that took about 2 s, all of it freeing memory the process was about to give
back; the agent of #178 found about three quarters of it in `bitwuzla_delete`
(node garbage collection, `malloc_consolidate`) and the rest in freeing
`Terms`' maps. The driver now ends every run that reached the solver through
`finish()`, which flushes `outs()`, `errs()` and the C streams and calls
`std::_Exit` with the same exit code (0 SUCCESSFUL, 1 FAILED or error,
2 UNKNOWN).

Nothing else needed a destructor. The witness file and the `--dump-dir`
files are `raw_fd_ostream`s local to the witness callback and to `dump()`, so
they are closed before the verdict. The binary makes no temporary files; the
front end removes its own after the binary exits. Nothing in Vereda writes
through C stdio, but a solver could, hence the `fflush(nullptr)`. LLVM's
`--time-passes` and `--stats` print nothing before or after: the pass
pipeline in `ir/load.cpp` registers no instrumentation, and statistics need
an LLVM built with assertions and are printed by `llvm_shutdown`, which Vereda
never calls. Two cases keep the old teardown. A run whose standard output
failed returns as before, so that `outs()`'s destructor still reports
"IO failure on output stream" and exits with 1 (checked with stdout on
`/dev/full`). A build with AddressSanitizer (`LLVM_ADDRESS_SANITIZER_BUILD`
from `llvm/Support/Compiler.h`) returns too, so that its leak check still sees
everything freed; in such a build the driver's only `_Exit` call left is the
out-of-memory handler's. A build with LeakSanitizer alone is not detected.

## Same output

The 298 tests of `tests/vereda` and 8 more cases (a witness to a file and to
a path that cannot be written, `--dump-smt-only` and `--dump-ir-only` with
`--dump-dir`, `--time-passes --stats`, `--incremental-bmc --multi-property`
ending without a verdict at its largest bound, `--incremental-bmc` with Z3, a
failure with cvc5) ran with both binaries in fresh directories: same exit
code, standard output, standard error and written files, with timings,
memory, temporary paths and the witness's uuid and creation time masked. So
were the outputs of the timed runs below, including the 87 MB counterexample
and 35 MB witness of the reach_error variant.

## Time

Release builds of main (a52266ba) and the branch, both with glibc's malloc
(no `DOWNLOAD_DEPENDENCIES`, so no mimalloc), run on the bitcode directly,
interleaved, 3 runs each, on the 80-core machine at a load average of about
20 from other agents. Wall time is with standard output to a file, as the
tests run; the time from the verdict line to exit comes from a second run
under a pty, where `outs()` is unbuffered and the verdict line arrives when
it is printed. Medians, ranges in brackets.

| run | wall, main | wall, branch | verdict to exit, main | verdict to exit, branch |
|---|---|---|---|---|
| `tests/esbmc/array_loop_summary_decrement_fail.c --no-summarize-array-loops` (FAILED) | 19.26 s (18.63-19.44) | 16.85 s (16.45-17.25) | 2.08 s (1.90-2.79) | 0.12 s (0.12-0.13) |
| the same program failing through reach_error, `--witness` to a file | 19.81 s (19.10-19.82) | 17.02 s (17.01-18.15) | 2.83 s (2.54-2.90) | 0.14 s (0.09-0.15) |
| `tests/vereda/float_arith.c --unwind 2` (SUCCESSFUL) | 0.063 s | 0.060 s | 0.007 s | 0.003 s |
| `tests/vereda/witness_violation.c --witness` to a file (FAILED) | 0.022 s | 0.022 s | 0.002 s | 0.002 s |

What is left after the verdict on the large runs, about 0.1 s, is the kernel
unmapping the 1 GB the process holds.

## Per-bound teardown under --incremental-bmc, not changed

Each bound still creates and destroys its own walk and solver. Measured as the
time between a "Bound k too small" line and the next "Checking bound" line,
over 30 s of `vereda.py --incremental-bmc --property-file unreach-call.prp`
(so `--unlimited-k-steps --k-step 2`) on ReachSafety tasks that run to the
time limit:

| task | bounds in 30 s | teardown | largest |
|---|---|---|---|
| loop-acceleration/array_2-1 | 620 | 3.78 s (12.6%) | 18 ms |
| array-examples/standard_vector_difference_ground | 588 | 4.15 s (13.8%) | 25 ms |
| loop-acceleration/array_2-2 (main) | 599 | 4.11 s (13.7%) | 16 ms |
| memory-model/2SB | 5 | under 1 ms | 1 ms |

On tasks that walk hundreds of cheap bounds the teardown is an eighth of the
run, so it matters; where the solver dominates, as on 2SB, it does not. A
flat profile of array_2-1 has glibc's free path (`free`, `_int_free_chunk`,
`malloc_consolidate`, `unlink_chunk`) at about 10% of the samples and
Bitwuzla's `NodeUniqueTable::erase` at 2%. mimalloc, which the CI and
ReachSafety builds link, may change that share; it was not measured.
Removing this teardown would not be a few lines: freeing nothing would grow
memory with every bound, and one solver kept across bounds was measured and
not kept (`2026-11-22-incremental-bmc.md`).
