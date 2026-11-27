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
// Imported from ESBMC regression/esbmc/cwe_heap_overflow_sarif
// Needs: --sarif-output
// RUN: --incremental-bmc --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: "ruleId": "heap-array-bounds-violated"
// EXPECT: "name": "CWE"
// EXPECT: "version": "4.20"
// EXPECT: "id": "122"
// Vereda: verdict none
