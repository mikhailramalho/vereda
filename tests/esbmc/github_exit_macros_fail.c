/* EXIT_SUCCESS is 0, so this does not hold. */
#include <assert.h>
#include <stdlib.h>

int main(void)
{
  assert(EXIT_SUCCESS == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_exit_macros_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
