#include <stdlib.h>

int main(void)
{
  int *p = malloc(sizeof(int));
  if (!p)
    return 0;
  p[3] = 42; /* OOB on a size-1 heap object -> Access to object out of bounds */
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_heap_scalar_oob
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: Access to object out of bounds: heap object
// EXPECT: ^  CWE: CWE-122, CWE-125, CWE-787, CWE-823$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
