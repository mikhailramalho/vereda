#include <stdlib.h>
#include <assert.h>

int main()
{
  void *a = (void *)malloc(4);
  void *b = (void *)malloc(-4);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_1631
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
