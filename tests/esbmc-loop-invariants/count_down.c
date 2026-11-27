int __VERIFIER_nondet_int();
int main()
{
  int fail = 0;
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0);
  __ESBMC_loop_invariant(x >= 0 && fail == 0);
  while (x > 0)
  {
    if (fail == 1)
    {
      x = -1;
      fail = 2;
    }
    else
    {
      x--;
    }
  }
  assert(x == 0);
}
// Imported from ESBMC regression/loop-invariants/count_down
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
