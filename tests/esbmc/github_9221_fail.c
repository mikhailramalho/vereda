/* #9221: q is allocated after p is freed, so it may reuse p's address. */
#include <stdint.h>
#include <stdlib.h>
int main(void)
{
  int *p = malloc(sizeof(int));
  if (!p)
    return 0;
  free(p);
  int *q = malloc(sizeof(int));
  if (!q)
    return 0;
  __VERIFIER_assert((uintptr_t)p != (uintptr_t)q); // q may reuse p's address
  free(q);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9221_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
