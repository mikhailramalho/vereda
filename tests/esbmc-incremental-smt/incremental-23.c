#include <assert.h>
int __VERIFIER_nondet_int();
int main()
{
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  int sum = a + b;
  int *c = &a;
  int *d = &b;
  while(a > 0)
  {
    *c = *c - 1;
    *d = *d + 1;
  }
  int sum2 = *c + *d;
  assert(sum != sum2);
  return 1;
}
// Imported from ESBMC regression/incremental-smt/incremental-23
// Needs: --k-induction
// RUN: --k-induction
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
