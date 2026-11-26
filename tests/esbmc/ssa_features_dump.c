unsigned __VERIFIER_nondet_uint(void);

int main()
{
  unsigned x = __VERIFIER_nondet_uint(), y = __VERIFIER_nondet_uint();
  unsigned m = x & y;
  __VERIFIER_assert(m != 6); // masked
  __VERIFIER_assert(x != 2); // operand
  return 0;
}
// Imported from ESBMC regression/esbmc/ssa_features_dump
// Needs: --ssa-features-dump
// RUN: --ssa-features-dump
// MODE: FUTURE
// EXPECT: ^SSA: Contains BITWISE OPERATIONS$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
