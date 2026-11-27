int __VERIFIER_nondet_int();

int main()
{
  int i = 0, x = 0, y = 0;
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0);
  for(i = 0; i < n; i++)
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
// Imported from ESBMC regression/incremental-smt/incremental-10
// Needs: --k-induction
// RUN: --k-induction
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
