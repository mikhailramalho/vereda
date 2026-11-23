#include <stdlib.h>

int main(void)
{
  int *p = malloc(4 * sizeof(int));
  if (!p)
    return 0;
  p[5] = 42; /* out-of-bounds write on a heap buffer (index 5, size 4) */
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_heap_overflow
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: array bounds violated: heap object
// EXPECT: ^  CWE: CWE-122, CWE-125, CWE-129, CWE-131, CWE-193, CWE-787$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
