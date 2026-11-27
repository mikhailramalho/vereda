#include <stdio.h>
int f(void) { return 0; }
int main()
{
  sscanf("1", "%d", (int *)(void *)f);
  return 0;
}
// Imported from ESBMC regression/esbmc/function_write_sscanf_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  dereference failure: Program code accessed in write or free mode$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: Program code accessed in write or free mode$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
