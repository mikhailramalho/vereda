unsigned int value(void)
{
  return 1;
}

int main(void)
{
  unsigned int x = value();
  __VERIFIER_assert(x != 1); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/uint_return_wide_negative
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m64 --std c11 --validate-violation-witness --witness witness.yml --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
