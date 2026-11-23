// --dead-code-check and --dead-store-check share the single --sarif-output
// path. Each advisory used to write its own SARIF document, so the second write
// truncated the first away (or, with "-", emitted two concatenated JSON
// documents). Both sets must land in one run (issue #4495).
int main(void)
{
  int x = 1;
  int unused = 42; // dead store, never read (CWE-563)
  if (x)
    return 0;
  else
    return 1; // dead branch, x is always 1 (CWE-561)
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_dead_store_sarif
// Needs: --dead-code-check --dead-store-check --sarif-output out.sarif CHECK_JSON out.sarif $.version == "2.1.0" CHECK_JSON out.sarif $.runs[0].results[0].ruleId == "dead-code" CHECK_JSON out.sarif $.runs[0].results[1].ruleId == "dead-store"
// RUN: --dead-code-check --dead-store-check --sarif-output out.sarif
// MODE: FUTURE
// EXPECT: :8: dead store: assignment to unused never read
// EXPECT: ^\[Dead code\]$
// EXPECT: :9: dead code: unreachable branch
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
