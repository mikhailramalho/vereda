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
// Needs: --smt-during-symex --smt-symex-assert --k-induction
// RUN: --smt-during-symex --smt-symex-assert --k-induction -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
