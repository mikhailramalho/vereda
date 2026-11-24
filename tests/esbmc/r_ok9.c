#include <assert.h>

int main()
{
  int a[10];
  int b[5];
  int *p;

  if (__VERIFIER_nondet_int())
    p = a;
  else
    p = b;

  assert(__ESBMC_r_ok(p, 20));
  assert(!__ESBMC_r_ok(p, 60));

  return 0;
}
// Imported from ESBMC regression/esbmc/r_ok9
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
