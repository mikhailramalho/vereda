#include <stdlib.h>
#include <assert.h>

int main()
{
  int result = atoi("2147483648"); // Maximum value of signed int + 1
  assert(result == 2147483647);
  
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1009-atoi-no-ovfl
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
