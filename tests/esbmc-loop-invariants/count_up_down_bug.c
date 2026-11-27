unsigned int __VERIFIER_nondet_uint();

int main()
{
  unsigned int n = __VERIFIER_nondet_uint();
  //  __VERIFIER_assume(n>0 && n<10000);
  unsigned int x = n, y = 0;
  //  __VERIFIER_assume(x==n);
  __ESBMC_loop_invariant(x + y == n);
  while (x > 0)
  {
    x--;
    y++;
  }
  assert(y != n);
  //  assert(x==0);
}
// Imported from ESBMC regression/loop-invariants/count_up_down_bug
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
