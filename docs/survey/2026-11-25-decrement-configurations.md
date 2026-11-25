# Exercise decrement tables with and without summaries

After PR #252 landed, merge main into PR #249 and keep the table-pattern
C++ unit target inside the libvereda guard. The source-only CMake fixture
and the production unit target both continue to work together.

The existing passing and failing imported decrement tests now each declare
two RUN directives: an empty one for the default summarized path, followed
by `--no-summarize-array-loops`. Their original names retain default-mode
coverage; `@run2` records the exact unrolled configuration that exposed the
non-incremental regression. Separate wrapper source files are unnecessary.
The importer emits both directives for these tests, and output-reader tests
check that re-importing preserves them, including translated 32-bit flags.

The focused eight-case CTest run passes: both decrement assertions under
both configurations, the output readers, configuration infrastructure,
table patterns and banner. The 14 configuration infrastructure tests also
pass with the actual combined CMake target declarations.

Using the newly landed benchmark support, run both unrolled fixtures with
the base binary built from the PR #252 code and with PR #249, using the
same LLVM 23.1.6 and camada 0.25. The base lacks the @run2 configurations,
so bench.py uses the head fixture on both sides and identifies that in its
report. With a 30-second baseline limit, the passing base times out while
head solves in 0.05 seconds (1.07 seconds wall, 159 MB peak). These are
local measurements; baseline resource-limited runs are not solve timings.

The failing configuration completes with both binaries and retains the
FAILED verdict. The confirmed comparison reports solve time 13.37 to
0.06 seconds, wall time 15.19 to 1.37 seconds, and peak memory 1,655 to
159 MB. bench.py reports no regressions for the two explicit configurations.

The merged full suite passes all 7,023 registered executions: 2,846 passes,
4,177 skips and no failures, in 196.50 seconds locally with eight concurrent
executions. Its per-configuration statistics separately record SUCCESSFUL
and FAILED for the two unrolled tests, both solving in 0.055 seconds.
