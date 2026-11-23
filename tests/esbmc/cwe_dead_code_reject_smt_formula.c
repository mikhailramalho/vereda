// --dead-code-check with --smt-formula-only emits the SMT formula (P_SMTLIB)
// without solving any probe, so every branch would be misreported as dead
// (issue #5934). The combination must be rejected up front.
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_smt_formula
// Needs: --dead-code-check
// RUN: --dead-code-check --smt-formula-only
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --smt-formula-only$
// Vereda: verdict none
