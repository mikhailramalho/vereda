#include <assert.h>
#include <stdlib.h>

/* A freed heap object no longer reserves its addresses, so a later
 * allocation may reuse them. */
int main(void)
{
  char *a = malloc(16);
  if (!a)
    return 0;
  unsigned long old = (unsigned long)a;
  free(a);
  char *b = malloc(16);
  if (!b)
    return 0;
  assert((unsigned long)b != old);
  return 0;
}
// Imported from ESBMC regression/esbmc/memspace_freed_heap_reuse_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
