int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 16 && x <= 32);
  int y = x >> 2;
  __VERIFIER_assert(y != 4); // x in [16,19] gives y == 4
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_ashr_bounds_fail
// Needs: --interval-analysis --interval-analysis-bitwise
// RUN: --interval-analysis --interval-analysis-bitwise --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
