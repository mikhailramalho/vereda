#include <stdlib.h>
#include <assert.h>

int main()
{
  void *a = (void *)malloc(4);
  void *b = (void *)malloc(-4);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_1631_compact
// Needs: --compact-trace
// RUN: --force-malloc-success --compact-trace
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
