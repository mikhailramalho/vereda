# Resume incremental BMC inside nested loops (#175)

## Scope and design

After #287, symbolic exits can resume, but only at an outer loop in the entry
function. A nested loop hitting the bound sets `truncatedInner`, so its prefix
is walked again at every bound. This is still quadratic work when SCEV cannot
provide a maximum trip count and the bounds grow one at a time.

A nested loop in the entry function can use the existing checkpoint without a
new call-stack representation: its `Frame` already contains the active enclosing
loops and their iteration counters. On resume, finish the interrupted loop, walk
the remainder of its parent region, then continue the parent from its next
iteration. Repeat outward until the function suffix has been walked or another
immediate checkpoint stops execution. `region` accepts the already-completed
child header so this shares its normal block traversal.

General checkpoints still replace the entire suffix, including unwinding checks,
claim mutations, trace steps, return arrivals, path facts and memory joins. A
nested general checkpoint additionally snapshots the encoder bindings: a suffix
can execute later outer iterations and overwrite the interrupted iteration’s
outer PHIs and other dominating SSA values. Restoring only the memory and loop
counters is insufficient. Immediate checkpoints need no binding snapshot because
no suffix runs. Fresh-symbol counters are not rewound.

The existing object-count and global-constraint-epoch checks remain in force.
Truncated callee loops and recursive cuts still prevent reuse, and checkpoints are excluded while
walking a synthetic array-summary iteration. Thus an outer prefix containing an
incomplete callee is never reused. No new flags or bound-growth policy are added.

## Controlled measurements

Base: `1c412ad8`, LLVM 23.1.6, camada 0.25 / Bitwuzla 0.11.0. Both binaries use
Clang 23, RelWithDebInfo and mimalloc 3.7.0, built in the same directory. Measurements
are separate serial processes, alternating base/head order over three trials,
with no compilation or regression suite running concurrently.

The fixture is `tests/vereda/incremental_resume_nested_deep.c`: three nested loops,
a step obtained from nondeterminism and assumed equal to one, two assertions and
post-loop computations. LLVM SCEV cannot count the symbolic-step inner loop even
though term folding knows the step. Array summaries are explicitly disabled.

```sh
python3 vereda.py --bin build/vereda tests/vereda/incremental_resume_nested_deep.c \
  -DSIZE=1000 --no-summarize-array-loops --incremental-bmc --unlimited-k-steps
```

At SIZE=1000 the median wall time falls from **2.186 s to 0.188 s** (11.6 times
faster), and median reported peak memory falls from **67 MB to 36 MB**. Both use
1,001 bounds and prove both assertions; the base restarts throughout and the head
resumes 1,000 times. These are controlled regression measurements, not a prediction
of a whole-suite speedup. Per-bound timing lines round to milliseconds, so their
sum is not used as the main timing metric.

The larger SIZE=2000 configuration is a separate `RUN` line so performance CI
compares the new fixture against the base binary too. Its median wall time falls
from **6.543 s to 0.253 s** (25.9 times faster), with peak memory **100 MB to
38 MB** and 2,000 resumes instead of restarts. The three wall samples are
6.560/6.498/6.543 s on the base and 0.253/0.254/0.253 s on the head.

## Validation

The Vereda suite completed 582 configurations with 581 passing and its existing
`lib_sprintf_overflow` skip. Additional 32-bit and large-fixture configurations are
covered by the focused incremental rerun. The 279 imported incremental fixtures
completed with 68 passing and 211 skipped, with no failures.

Regression coverage includes three-level continuations and parent suffixes,
symbolic exits with overwritten outer bindings, early returns and sibling paths,
path-local memory and uninitialised-read checking, stable property IDs, and a bug
first reachable in a later outer iteration. The existing nested truncation tests
now require resumption rather than forbidding it. A callee-cut test preserves the
fallback boundary.

An isolated variant with only binding restoration removed falsely fails
`incremental_resume_nested_bindings.c`; the actual change proves it. The failing
nested-loop test’s counterexample and witness match the base output after removing
only generated witness UUID and creation time.

A small SV-COMP screen used unreach-call, ILP32 and incremental BMC. The three
terminating cases from the original issue retain their verdicts:
`standard_vector_difference_ground` and `array_2-2` prove, while `array_2-1`
fails. `partial_lesser_bound-1` still does not finish within the 10-second screen.
Two large nested cases, `nested_1-1` and `nested5-1`, also remain incomplete at
10 seconds on both binaries. `nested_delay_notd2` finds the same violation, now
with five resumes instead of none. This is only a smoke screen, not the longer
ReachSafety gate requested by the issue.

## Remaining work

This implements nested-loop continuation within the entry function, not a general
continuation through calls or recursion. Suffix allocation/constraint rollback and
the longer ReachSafety decision gate remain open in #175. The global-counter
`partial_lesser_bound-1` case still needs a suitable loop summary rather than
millions of explicit iterations.
