# Recover coverage hidden by imported test metadata

Audit build/log from main 394cbd6e: 7,023 registered executions, 2,846
passes and 4,177 skips. The skipped tests comprise 4,100 FUTURE and 77
KNOWNBUG fixtures. Correlating the log with the headers and LastTest.log
gives:

| skipped outcome | count |
|---|---:|
| no verifier verdict, mostly unsupported options | 3,010 |
| unsupported builtin, skipped before execution | 380 |
| SUCCESSFUL with unmet expectations | 322 |
| FAILED with unmet expectations | 194 |
| UNKNOWN with unmet expectations | 271 |

Only 108 of the skipped fixtures already have the expected verdict. A
matching verdict is insufficient to promote them: some require checks the
verifier does not implement, such as heap bounds checks; others require
different trace details. Keep these expectations instead of deleting them
to lower the skip count.

## Target requirements

An imported REQUIRES arch32 directive was always marked unsupported, even
with an explicit --32 option. That also selected the fallback that writes
the original command line, restoring --32 instead of its supported -m32
translation. Recognize arch32 only with explicit -m32, and arch64 only with
explicit -m64. Unspecified, conflicting and other requirements remain
unsupported. The importer does not guess the host or target layout.

This makes 28 fixtures satisfy all their existing expectations, including
six that use Z3 for floating-point precision and regression checks.

## Numeric selectors and fallback commands

--floatbv was already in DROPPED, but the unsupported-option/requirement
fallback copied raw arguments and restored it in checked-in RUN lines.
Drop --fixedbv as requested too, and remove both numeric selectors from
fallback RUN lines even when other unsupported requirements remain.
Token filtering preserves the raw quoting of other options and macro data.
All 223 affected original argument strings retain every other decoded token.

Refresh existing metadata as well as fixing future imports: none of the
imported C/IR RUN headers retain either selector. For example,
esbmc-floats/int-to-float-prec-boundary now runs --solver z3 -m32 and no
longer declares an unsupported arch32 requirement. Twenty-six additional
fixtures pass after removing obsolete encoding selectors. The verifier's
supported floating-point encoding is used; cases whose expectations still
do not hold remain FUTURE.

## Z3-specific coverage

The esbmc-z3 suite has 73 skips. Sixty-six need quantifier or invariant
builtins, four need transition-system PDR, and one needs heap bounds checks.
These are frontend/verification capabilities, not a missing Z3 backend.
The two remaining fixtures, github_9229 and github_9229_fail, already
produce the correct overflow verdicts with Z3.

Translate their ESBMC result-row status PASSED to Vereda's SUCCESS, allow
the different spacing before the same line number, and translate binary
!overflow expressions to the corresponding arithmetic-overflow message.
Keep the success/failure and overflow-operation expectations. Restrict
status translation to result-looking rows, leaving ordinary PASSED messages
alone. The tests keep --solver z3; eight recovered fixtures in total use Z3.
Quantifier, invariant, PDR and bounds tests retain their requirements.

## Validation and promotion

Re-run 187 candidate fixtures after refreshing the metadata. Fifty-five
satisfy every EXPECT and EXPECT-NOT, 130 still have unmet expectations,
and two exceed a 30-second probe limit. Promote only the 55 complete
matches, and update recorded verdicts for changed configurations that
remain FUTURE. Do not change source bodies, drop expectations, swap
solvers, or alter unwind bounds to make tests pass. The full 300-second run
also confirms that 01_cbmc_Fixedbv11 satisfies its FAILED expectation after
about 193 seconds, so it is promoted too: 56 recovered fixtures in total.

The other short-probe timeout, esbmc/github_243, reaches the solver on its
32-bit target and exhausts the 4 GiB limit after about 260 seconds. Record
this as a KNOWNBUG in the fixture and importer, preserving its FAILED
expectation. This exposes a solver resource limit that the former invalid
--32 command never reached; it is not counted among the recovered tests.

The metadata refresh changes 310 fixtures. The larger file count includes
removing obsolete numeric flags from still-unsupported configurations and
resolving arch32 within combined requirements. A comparison against HEAD
confirms zero source-body changes. Expectation changes on still-failing
fixtures are avoided; the two Z3 output translations preserve the tested
status, location where specified, and overflow operation.

output_readers.py covers explicit/missing/conflicting architecture
requirements, both discarded numeric selectors, the raw-argument fallback,
the reported Z3 precision example, and result/overflow output translations.

The full suite initially has 2,901 passes, 4,120 skips and two failures:
the newly observed kernel memory limit and the slow Fixedbv11 fixture whose
correct verdict was not available within the short probe. After recording
the former as KNOWNBUG and promoting the latter, rerun those two cases:
Fixedbv11 passes in 172.85 seconds and github_243 skips for its known memory
limit in 202.69 seconds, with no failures. Together with the unchanged
executions from the full run, the result is 2,902 passes and 4,121 skips:
56 fewer skips than build/log. The focused eight-case run for the importer
and recovered Z3 tests also passes.
