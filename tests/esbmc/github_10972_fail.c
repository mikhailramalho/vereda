#include <stdlib.h>

int main()
{
  free("a"); // #10972
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10972_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: free\(\) of non-dynamic memory
