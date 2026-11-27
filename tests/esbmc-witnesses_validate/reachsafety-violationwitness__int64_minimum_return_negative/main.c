long long value(void)
{
  return -9223372036854775807LL - 1;
}

int main(void)
{
  long long x = value();
  __VERIFIER_assert(x != (-9223372036854775807LL - 1)); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/int64_minimum_return_negative
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m64 --std c11 --validate-violation-witness --witness witness.yml --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
