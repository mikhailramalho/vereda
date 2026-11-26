extern _Bool __VERIFIER_nondet_bool(void);
int main(void)
{
  _Bool value = __VERIFIER_nondet_bool();
  __VERIFIER_assert(!value); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/bool_return_negative
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m64 --validate-violation-witness --witness witness.yml
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
