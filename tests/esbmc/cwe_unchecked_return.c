#include <stdlib.h>

int main(void)
{
  int *p = calloc(1, sizeof(int));
  *p = 42;
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_unchecked_return
// Needs: --unchecked-return-value-check
// RUN: --unchecked-return-value-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  unchecked return value of calloc: p$
// EXPECT: ^  CWE: CWE-252$
// Vereda: verdict none
