#include <stdlib.h>

void reach_error(void)
{
  __VERIFIER_assert(0); // NULL alternative of malloc(0) reached
}

int main(void)
{
  void *p = malloc(0);
  if ((unsigned long)p == (unsigned long)0)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5398_null_fail
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
