
int __VERIFIER_nondet_int();

int main()
{
  int i = 0, x = 0, y = 0;
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0);
  __ESBMC_loop_invariant(i <= n && x == 0);
  for (i = 0; i < n; i++)
  {
    x = x - y;
    assert(x == 0);
    y = __VERIFIER_nondet_int();
    __VERIFIER_assume(y != 0);
    x = x + y;
    assert(x != 0);
  }
  //__VERIFIER_assume(i>=n);
  assert(x == 0);
}
// Imported from ESBMC regression/loop-invariants/for_bounded_loop1
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
