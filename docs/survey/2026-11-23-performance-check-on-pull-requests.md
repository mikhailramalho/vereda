# Performance check on pull requests

Recorded 2026-11-23.

68dfce6f took 10_fir_new from a 21 s run to a solve that does not end, and
no test failed: the imported suites skip a run past their time limit, so
tests.yml only sees verdicts. `.github/workflows/perf.yml` now builds a
pull request's base and head on one runner and `tests/bench.py` runs
the tests in `tests/bench.txt` with both binaries side by side, so the
runner's noise hits both alike, and compares them relatively: head fails
when it is 1.5 times slower and 2 s more, uses 1.5 times the memory and
200 MB more, times out or is killed for memory where base was not, gives
another verdict or none. A time or memory regression runs once more and
counts only if it shows again.

The 23 tests were picked from the 160 with the longest ctest times by
running each at 3b0eab7c, one per stage: floating point (10_fir_new,
10_bicycle_01, Float27, math_log02), memory (array_loop_summary_counter_fail,
the only one of its family under a minute, at 40 s and 1.4 GB;
memset_whole_large_buffer), the walker (github_306), Z3 (github_2312_5),
vectors and function pointers. Many candidates time out at 60 s or stop at
an unsupported construct; deep_binary_chain_pass takes 20 s, but in clang.
At 3b0eab7c a run of the list against itself took about 4.5 minutes on an
80-core machine, two of them in 10_fir_new, whose solve grew until the OOM
killer stopped it; since f4e1abde kept floats as FP terms it takes 0.7 s,
and at ee26cffa the list takes 1 m 41 s against itself and flags nothing.
Several tests became much faster with those commits (math_log02,
github_1600, Float27 under 0.2 s); they stay, as cheap guards against a
large slowdown.

Comparing 1266dcdf with 68dfce6f flags 10_fir_new (killed for memory after
137 s, 40 s before), math_log02 (x9), native_fp_arith_fail (x12) and
github_306 (walk 0.06 s to 6.4 s), all from the solver's FP theory, and
array_of_vector_symbolic, UNKNOWN before vectors and SUCCESSFUL after.
Float27 slows from 0.7 s to 2.4 s, under the 2 s margin. With ee26cffa as
base and 3b0eab7c as head, it flags 10_fir_new (head timed out),
math_log02 (x16) and github_1600 (x20, memory x4.8), the work of
f4e1abde and ee26cffa seen backwards.
