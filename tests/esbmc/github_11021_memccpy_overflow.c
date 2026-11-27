#include <string.h>
int main()
{
  char b[2];
  memccpy(b, "abcd", 'z', 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_11021_memccpy_overflow
// RUN: --solver z3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
