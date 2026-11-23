#include <limits.h>

int main(void)
{
  int x = INT_MAX;
  return x + 1;
}
// Imported from ESBMC regression/esbmc/cwe_overflow
// RUN: --overflow-check --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  arithmetic overflow on add$
// EXPECT: ^  CWE: CWE-190, CWE-191$
// Vereda: unmet EXPECT: ^  CWE: CWE-190, CWE-191$
// MODE: FUTURE
// Vereda: verdict FAILED
