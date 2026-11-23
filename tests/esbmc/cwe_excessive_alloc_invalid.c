#include <stdlib.h>

int main(void)
{
  char *p = malloc(8);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_invalid
// Needs: --excessive-alloc-check=0
// RUN: --excessive-alloc-check=0 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --excessive-alloc-check=K requires K >= 1 \(got 0\)$
// Vereda: verdict none
