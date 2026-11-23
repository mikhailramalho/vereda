#include <stdlib.h>

extern void __VERIFIER_assume(int);

int main(void)
{
  void *p = malloc(0);
  __VERIFIER_assume((unsigned long)p != (unsigned long)0);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5398
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
