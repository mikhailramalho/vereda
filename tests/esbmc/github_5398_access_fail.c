#include <stdlib.h>

extern void __VERIFIER_assume(int);

int main(void)
{
  char *p = malloc(0);
  __VERIFIER_assume((unsigned long)p != (unsigned long)0);
  *p = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5398_access_fail
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*dereference failure: array bounds violated: heap object$
// Vereda: verdict none
