// #10383: a VLA with an empty initializer is zero-initialised.
int __VERIFIER_nondet_int(void);
int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0 && n < 4);
  int a[n] = {};
  __VERIFIER_assert(a[0] == 0 && a[n - 1] == 0); // zero
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10383
// RUN: --std c2x --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
