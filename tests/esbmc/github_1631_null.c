#include <stdlib.h>
#include <assert.h>

int main()
{
  void *b = (void *)malloc(-4);
  assert(b == NULL);
}
// Imported from ESBMC regression/esbmc/github_1631_null
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
