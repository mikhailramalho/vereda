// --dead-code-check and the other coverage modes share goto_coveraget's
// all_claims and multi_property routing; combining them (here with
// --assertion-coverage) would divert or clobber the branch probes and make the
// reporter flag live branches as dead (issue #4495). Reject up front.
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_coverage
// Needs: --dead-code-check --assertion-coverage
// RUN: --dead-code-check --assertion-coverage
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --assertion-coverage$
// Vereda: verdict none
