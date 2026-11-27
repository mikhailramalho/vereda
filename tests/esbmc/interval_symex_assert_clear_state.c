// A write through a pointer resets the interval domain to the shared empty map.
// y = 3 must not be written into that shared map, or the second reset would
// bring the stale interval back and prune the claim.
int __VERIFIER_nondet_int();
int main()
{
  int y, z;
  int *p = &y;
  z = 1;
  y = 3;
  *p = __VERIFIER_nondet_int();
  y = 3;
  *p = __VERIFIER_nondet_int();
  __VERIFIER_assert(y == 3); // y still 3
}
// Imported from ESBMC regression/esbmc/interval_symex_assert_clear_state
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
