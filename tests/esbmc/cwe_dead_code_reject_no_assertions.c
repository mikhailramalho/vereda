// The branch probes inserted by goto_coveraget::insert_assert are marked
// user_provided(true), which is exactly what goto_symext::symex_assert drops
// under --no-assertions. That leaves reached_claims empty while all_claims
// still holds every probe, so both branches below would be reported as
// CWE-561 (issue #4495). The combination must be rejected up front.
int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_no_assertions
// Needs: --dead-code-check --no-assertions
// RUN: --dead-code-check --no-assertions
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --no-assertions$
// Vereda: verdict none
