unsigned int __VERIFIER_nondet_uint();

int main()
{
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned int x = n, y = 0;
  __ESBMC_loop_invariant(x + y == n);
  while (x > 0)
  {
    x--;
    y++;
  }
  assert(y == n);
}
// Imported from ESBMC regression/loop-invariants/count_up_down
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
