// --dead-code-check with --claim leaves every unselected assertion SKIP but
// still in all_claims, so its probe never solves and a live branch would be
// misreported as dead (issue #4495). The combination must be rejected up front.
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_claim
// Needs: --dead-code-check --claim 1
// RUN: --dead-code-check --claim 1 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --claim$
// Vereda: verdict none
