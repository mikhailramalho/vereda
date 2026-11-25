# Consistent console output

Recorded 2026-11-25.

The Picosat example mixed tagged progress with untagged timings and loop
iterations, C comments in counterexamples, and Markdown-style result markers.
Runs with no remaining checks skipped the results section altogether.

The console now uses three log tags: PROGRESS for activity (including loop
iterations), INFO for target, stage timings, memory, summaries and written
files, and WARNING for warnings. Compiler diagnostics retain their native
format. The version banner stays at the top.

COUNTEREXAMPLE and RESULTS are uppercase bracketed section headings. The
counterexample has plain assumption and state blocks, and grouped binary
values on a separate bits line. The final violated-property block names its
location once, instead of repeating it in a synthetic final state and a C
block comment. State assignments retain their existing order and numbers.

Every decided run prints RESULTS and the solver's checked, passed, failed
and not-checked counts, including a zero-count summary when no checks remain.
UNKNOWN prints RESULTS and its existing reason-bearing verdict, without
inventing property counts. Intermediate incremental bounds do not print a
final results section. Dump-only commands still print their requested data.

The final VERIFICATION and SV-COMP verdict lines, exit codes, property IDs
and witness files are unchanged. Other console text is intentionally changed;
external consumers of that text need to adopt the new layout. The bundled
statistics reader accepts both old and new timing lines so comparisons with
older commits keep working. The ESBMC importer translates expectations to the
new trace and summary layout.

Validation: LLVM 23 Release build, 432 Vereda tests with one existing skip;
the expectation mismatches from the first run were corrected and the affected
tests rerun. All 56 affected imported fixtures were checked with a 20-second
limit: 9 passed and 47 skipped, none failed. The full imported suite was not
run. A small reader test covers old and new timings and imported output
patterns; source tests cover zero checks and UNKNOWN.

The original three-file Picosat command with --unwind 2 still fails its first
unwinding assertion. Its new summary is:

```text
[RESULTS]
.../main.c, function main
  FAILED       [main.unwind.0]  line 79  unwinding assertion loop 0

Properties: 1 checked, 0 passed, 1 failed, 3 not checked

VERIFICATION FAILED
```

The reach_true C test reports two checked and passed properties and the same
results section before VERIFICATION SUCCESSFUL. Timings in the demonstration
are not a performance comparison.
