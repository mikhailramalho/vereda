int __VERIFIER_nondet_int();

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0);
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(y >= 0);
  __VERIFIER_assume(y < 1);
  int z = 0;

  __ESBMC_loop_invariant(
    (x >= 0) && (y >= 0) && ((x > y && z == 0) || (x <= y && z == 1)));
  while (x > 0)
  {
    x--;
    if (x == y)
      z = 1; // assert(0);
  }
  assert(z == 0);
}
// Imported from ESBMC regression/loop-invariants/check_if_bug
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
