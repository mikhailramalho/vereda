#include <stdlib.h>
#include <assert.h>

int main()
{
  char* s = NULL;
  int result = atoi(s);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1009_fail3
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
