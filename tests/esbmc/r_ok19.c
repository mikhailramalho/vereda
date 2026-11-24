#include <assert.h>

int main()
{
  char a[10];
  char b[5];
  char *p;

  int nondet;
  __VERIFIER_assume(nondet == 1);
  if (nondet == 1) {
    p = a;
  }
  else {
    p = b;
  }

  assert(__ESBMC_r_ok(p, 6));
  return 0;
}
// Imported from ESBMC regression/esbmc/r_ok19
// Needs: __ESBMC_r_ok
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
