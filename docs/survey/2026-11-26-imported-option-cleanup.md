# Imported option cleanup

Remove obsolete ESBMC pipeline, trace, external solver, transition-system and
dump options from imported regression commands. Options taking an argument are
removed together with their argument, including quoted external solver commands.
Apply the same rules to fallback commands when another unsupported feature keeps
a test FUTURE. Normalize --32/--64 to -m32/-m64 and --data-races-check to the
canonical --data-race-check, including fallback commands.

CHERI fixtures require different pointer semantics and must not be treated as
ordinary C after discarding --cheri. Exclude these imports explicitly, along with
ESBMC frontend internals, Lean export, transition-system engines and experimental
solver wrappers. Removing a backend flag is not enough to turn its own tests into
useful Vereda verification tests. Remove
dump-only fixtures without a verification verdict and strip dump expectations
from retained verification fixtures. Vereda's own SMT dump tests continue to
exercise --dump-smt-only; only imported uses are removed.

The migration changes 774 fixtures and removes 288 ESBMC-specific or output-only
fixtures. Retained C bodies are unchanged. Existing modes are rechecked separately
instead of promoting tests merely because an unsupported flag disappeared.

Drop the additional GOTO GCSE, field-sensitive SSA, scalarization, inlining and
pruning controls as ESBMC implementation choices. Drop result/trace formatting,
color and verbosity options, consuming the verbosity value. Little-endian is the
supported target default. Normalize --func, --unlimited-k-step,
--no-unwinding-assertion and --no-standard-check to their canonical spellings.
This does not add --no-standard-checks or any other mode to the binary: unsupported
canonical controls remain FUTURE requirements until implemented separately.

Add tests/list_options.py to count all regression RUN option occurrences, including
skipped tests and independent configurations. It groups equals-attached values
under the option name, excludes negative numeric values, and reports malformed
shell quoting with the file and line number.

Validation uses the unchanged main verifier with LLVM 23.1.6. With three workers
and a 15-second per-run limit, the 774 changed fixtures finish with 127 passes,
647 skips and no failures after promoting 127 fixtures to CORE. Timeout cases
retain their prior modes. The importer/output-reader checks and all 14
test-configuration tests pass. Audits find no removed options in imported RUN
headers and no edits to retained C bodies. Full 300-second coverage remains for CI.

Follow-up issues: #310.
