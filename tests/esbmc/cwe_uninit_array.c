#include <stdio.h>

int main(void)
{
  int scores[5];
  int sum = 0;
  for (int i = 0; i < 5; i++)
    sum += scores[i];
  printf("%d\n", sum);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array
// RUN: --uninitialised-vars-check --incremental-bmc
// Vereda: unmet EXPECT: ^  CWE: CWE-457$ (Vereda prints no CWE line)
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  use of uninitialized variable: scores$
// EXPECT: ^  CWE: CWE-457$
// Vereda: verdict FAILED
