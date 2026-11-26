// #10383: a VLA with an empty initializer is zero-initialised.
int __VERIFIER_nondet_int(void);
int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0 && n < 4);
  int a[n] = {};
  __VERIFIER_assert(a[n - 1] != 0); // nonzero
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10383_fail
// RUN: --std c2x
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
