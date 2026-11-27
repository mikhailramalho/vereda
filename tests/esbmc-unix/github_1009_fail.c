#include <stdlib.h>
#include <assert.h>

int main()
{
  int APPLE = atoi("-4451");
  assert(APPLE == 4451);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1009_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
