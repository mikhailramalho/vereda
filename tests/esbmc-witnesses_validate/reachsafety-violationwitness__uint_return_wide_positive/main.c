unsigned int value(void)
{
  return 1;
}

int main(void)
{
  unsigned int x = value();
  __VERIFIER_assert(x != 1); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/uint_return_wide_positive
// Needs: --std c11 --validate-violation-witness --witness witness.yml
// RUN: --64 --std c11 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
