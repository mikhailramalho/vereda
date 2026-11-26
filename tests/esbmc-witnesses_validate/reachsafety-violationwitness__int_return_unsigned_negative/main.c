int value(void)
{
  return 1;
}

int main(void)
{
  int x = value();
  __VERIFIER_assert(x != 1); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/int_return_unsigned_negative
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m64 --std c11 --validate-violation-witness --witness witness.yml
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
