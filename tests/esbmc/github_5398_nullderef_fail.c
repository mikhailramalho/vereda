// The NULL alternative of malloc(0) must stay a genuine NULL pointer: the
// allocation tracking arrays are indexed by the dynamic object, never by the
// conditional result, or __ESBMC_alloc[NULL] is set and this deref is missed.
#include <stdlib.h>

int main(void)
{
  char *p = malloc(0);
  if (p == 0)
    *p = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5398_nullderef_fail
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*dereference failure: NULL pointer$
// Vereda: verdict none
