#include <assert.h>
int main()
{
  do
  {
  } while (1);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_2602_1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
