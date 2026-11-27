// After a write through a pointer resets the interval domain, a later assume
// is tracked again and discharges the claim.
int __VERIFIER_nondet_int();
int main()
{
  int y, z;
  int *p = &y;
  z = 1;
  *p = __VERIFIER_nondet_int();
  __VERIFIER_assume(y >= 0 && y <= 5);
  __VERIFIER_assert(y <= 10); // y at most 10
}
// Imported from ESBMC regression/esbmc/interval_symex_assert_clear_state_prune
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
