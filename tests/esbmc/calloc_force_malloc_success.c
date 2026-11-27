#include <assert.h>
#include <stdlib.h>
int main()
{
  int *p = calloc(1, sizeof(int));
  assert(p != NULL);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/calloc_force_malloc_success
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
