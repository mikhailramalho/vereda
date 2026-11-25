# Independent regression configurations and new performance fixtures

The non-incremental decrement regression investigated in PR #249 was absent
from performance CI because the source tests ran with loop summaries enabled.
The failing configuration explicitly disabled summaries. Adding another RUN
line previously overwrote the first one, rather than testing both modes.
Separately, performance selection considered only names present in both suite
statistics, and `bench.py` skipped a source absent from the base tree. A new
regression therefore could not demonstrate its improvement in its own PR.

## RUN semantics

Each RUN directive now describes an independent execution, with all EXPECT,
EXPECT-NOT, MODE, recorded verdict, FILES and CWD directives shared. A file
without RUN still executes once with default flags. To test both default
flags and another mode, write an empty RUN followed by the additional flags:

```c
// RUN:
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
```

CTest registers each execution separately, preserving the existing name for
the first and appending `@run2`, `@run3`, etc. Additional configurations also
have separate statistics files with these suffixes. Every execution gets its
own timeout and resource limits; expectations and KNOWNBUG promotion are
checked independently. Editing a source header triggers CMake reconfiguration
so adding or removing a RUN changes registration on the next build.

Running `tests/run.py` directly without `--run-index` executes every RUN in
separate child processes. This keeps peak-resource accounting independent.
Failures take precedence over skips and later successes, and memory failures
retain the special exit status that prevents `import_esbmc.py --mark` from
rewriting modes. One-based `--run-index` selects a particular execution.
Expectations and recorded verdicts remain shared; different expected verdicts
still need separate fixtures.

## Performance comparisons

The statistics screen now includes head-only tests/configurations with a
verification verdict or resource-limit outcome, in addition to existing
time/memory changes. New cases are ranked first, within the existing cap of
40 candidates. `@runN` selects the same execution in explicit benchmark lists.

When the base lacks the source or selected RUN, both binaries use the head's
source fixture, flags and auxiliary sources. A changed flag configuration
also uses the head fixture on both sides; a recorded base timeout for the
older configuration is not reused. Each binary still uses its own tree's
front end. `%S` and CWD are respected. The benchmark table identifies when
base uses the head fixture. If the new fixture has a different verdict on
base (including an unsupported configuration), show the outcomes and mark
performance as incomparable. New feature tests need not work on the older
binary; such a difference is neither a measured speedup nor a performance
failure. A missing verdict on head remains a failure. Resource-limit
comparisons retain their existing treatment.

Existing unchanged configurations continue to use their respective fixtures.
The wall/memory thresholds, side-by-side measurements and repeated
confirmation of improvements/regressions are unchanged. No solver behavior
changes are included in this work.

## Validation

All 13 Python tests pass. They exercise default/C/IR directives, shared expectations,
separate commands and statistics, explicit run selection, invalid indices,
timeouts, memory failures, KNOWNBUG promotion, new candidate selection,
head-fixture source/flag/auxiliary-file/CWD selection and solver errors.
A small generated CMake project uses the real registration code to verify
multiple C and IR RUN cases and automatic reconfiguration after a header edit.
The existing nested-loop regression also runs under both solver modes.

A real-binary comparison of `vereda/loop_nested@run2`, absent from the base's
RUN configurations, executes both binaries successfully using the head
fixture (about 0.10 seconds wall time each, 28 MB peak memory). This checks
the fallback through the actual front ends, rather than only the fake
front ends used to isolate runner behavior in the Python tests.

The full suite has 7,010 registered executions: 2,833 passes, 4,177 skips
and no failures (191.81 seconds locally, eight executions at a time).
