#include <stdlib.h>

int main(void)
{
  int *p = calloc(1, sizeof(int));
  if (p == 0)
    return 1;
  *p = 42;
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_unchecked_return_pass
// Needs: --unchecked-return-value-check
// RUN: --unchecked-return-value-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
