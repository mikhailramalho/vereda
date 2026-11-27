#include <string.h>
int main()
{
  char b[2];
  stpcpy(b, "abcd");
  return 0;
}
// Imported from ESBMC regression/esbmc/github_11021_stpcpy_overflow
// RUN: --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
