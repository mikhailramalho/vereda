# File names in locations, built once

Recorded 2026-11-24.

A profile of the walk on array_loop_summary_decrement_fail with
`--no-summarize-array-loops` (100,000 iterations) found about 1% of its
samples in `ir::joinedPath`, `ir::fileName` and `ir::SourceLoc`. Every trace
step and check gets a `SourceLoc`, and `fileName` built its file's name
anew each time: it joined the location's directory and file name, and the
compile unit's, with `sys::path::append` into two strings, to compare them
(#85). `dbgAssign` does this for every debug record of every walked block,
also those whose value the trace then skips.

The name depends only on the location's `DIFile` and its compile unit, both
uniqued metadata that live as long as the driver's one `LLVMContext`, so
`fileName` now takes the `DIFile` and keeps each pair's name in a
`DenseMap`. `block()` moves a `DbgAssign`'s location and name into its step
instead of copying them.

## Time

Release builds with the installed LLVM 21, camada 0.24 and glibc's malloc
(no `MIMALLOC_SOURCE_DIR`), main at a52266ba against this change; the
machine has 80 cores and ran under a load of about 25-35.

Base and head run alternately on the benchmark's IR, three runs each: walk
times 8.49, 8.00, 8.75 s against 7.68, 7.56, 7.68 s (medians 8.49 and
7.68 s). Counted by perf, the walk plus `--dump-smt-only` takes 6% fewer
user instructions (17.98 to 16.89 billion) and 7% fewer user cycles (19.43
to 18.05 billion); a whole run 2.4% fewer instructions (46.23 to 45.13
billion). Verdict and peak memory do not change. `joinedPath` and the
`sys::path` functions are gone from the profile; `fileName` falls from 0.18%
to 0.03% of samples, plus 0.03% in the map.

Output is byte-identical: the vereda suite and every test matching
`witness|cex|counterexample|trace|dump` (545 tests, `.i` inputs with
linemarkers and multi-file programs among them), run with both binaries,
print the same and write the same witness and dump files, once times, peak
memory, witness uuids and temporary paths are masked.

## Not done

A step still owns its file and function names as `std::string`s, one copy
each. Interning them (a `StringRef` into the cache) would change every
reader (the driver's summary, `checks::Check::id`, the counterexample and
the witness), which concatenate and look them up as strings, for little:
`Step::Step`, copies included, is 0.10% of the samples.
