int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 16 && x <= 32);
  int y = x >> 2;
  __VERIFIER_assert(y >= 4 && y <= 8); // x >> 2 is in [4,8] for x in [16,32]
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_ashr_bounds
// Needs: --interval-analysis --interval-analysis-bitwise
// RUN: --interval-analysis --interval-analysis-bitwise
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
