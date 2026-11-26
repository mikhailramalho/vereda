# Retire ESBMC implementation flags and dump expectations

Imported C verification tests no longer require ESBMC's
`--tuple-sym-flattener`, `--state-hashing`, `--smt-during-symex`,
`--add-symex-value-sets`, or `--goto-functions-only`. These select ESBMC's
implementation or output and are not Vereda verification capabilities.
The importer drops them in translated commands and in raw fallback commands
that still contain unsupported features. Numeric selectors retain the same
handling. Exact decoded tokens, including boolean-valued forms, are removed;
quoted macro values containing a flag name remain unchanged.

Imported tests use Vereda's normal solver configuration. Vereda's own
`--incremental-solver` remains the way to request incremental solving.
No verifier implementation or command-line aliases are added here.

## Dump-mode expectations

When the original command requests `--smt-formula-only`, `--goto-program-only`,
`--goto-functions-only`, `--program-only`, or their `-too` variants, filter
expectations tied to ESBMC's GOTO, SSA and SMT representations. This includes
instruction dumps, synthetic symbol names, solver syntax, output-file contents
and dump-related messages. Retain verification verdicts and counterexample
checks. This filtering is conditional on a dump mode; other tests retain their
existing expectations. Legacy program-output switches are ignored, while the
existing SMT dump translations remain in place.

For assertion result rows, remove a known ESBMC assertion message while keeping
the failed claim id and source line. This recovers github_5934-solved without
turning its second-assertion regression into a verdict-only test.

A dump test with no remaining positive verification verdict is excluded from
future imports, and a stale prior copy is removed. The 80 existing dump-mode
fixtures contain 78 tests of ESBMC's printed representations alone and two
with verification verdicts. Remove the 78 output-only fixtures, including two
that had no retained expectations already, and retain both verdict tests.
This also removes an unbounded symbolic loop used only to inspect GOTO output.
It exceeded the short probe limit after its output flag was dropped; verifying
that loop is not the purpose of that fixture.

## Recovered coverage and validation

Initially refresh 270 headers mentioning the retired flags. Of the 28 cases
with no remaining declared blocker, eight satisfy all existing expectations:
github_1655, github_6540, github_6540_all_pass, github_6540_last,
github_7855-byte-update-sds and its failing pair, incremental-smt/incremental-21,
and parallel-solving/github_10103_smt_during_symex_multi_property. Promote those
and github_5934-solved to CORE. Remaining cases preserve their requirements and
recorded outcomes; unsupported ESBMC/CPROVER builtins remain unsupported.

Retained program bodies are unchanged. Tests cover all five retired options,
boolean spellings, raw fallback commands, quoted macro text, empty fallback RUN
lines, every requested dump-mode family and suffix, preservation of verdict and
counterexample checks, assertion-row labels, and removal of a stale output-only
fixture. A non-dump control retains its original representation expectation.

The full LLVM 23.1.6 Release run covers 6,953 executions. Its sole remaining
failure was github_5934-solved passing while still marked FUTURE; after promotion,
the failed-test rerun passes. Final classification is 2,923 passes and 4,030
expected skips, with no outstanding failures. The full run took 346.99 seconds
with eight workers. Importer and registration tests pass, and all nine promoted
fixtures also pass with PR #257's frontend and verifier. Audit confirms that
78 deleted fixtures are output-only, retained C bodies are unchanged, retired
option tokens are absent from headers, and `git diff --check` passes.
