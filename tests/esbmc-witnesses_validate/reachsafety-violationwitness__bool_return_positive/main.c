extern _Bool __VERIFIER_nondet_bool(void);
int main(void)
{
  _Bool value = __VERIFIER_nondet_bool();
  __VERIFIER_assert(!value); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/bool_return_positive
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --64 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
