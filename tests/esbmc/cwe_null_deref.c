#include <stdlib.h>

int main(void)
{
  int *p = NULL;
  return *p;
}
// Imported from ESBMC regression/esbmc/cwe_null_deref
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: NULL pointer$
// EXPECT: ^  CWE: CWE-476$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
