/* The pre-cap reading: an allocation above PTRDIFF_MAX succeeded. Asserting
   that must now fail. */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

int main(void)
{
  char *over = malloc((size_t)PTRDIFF_MAX + 1);
  assert(over != NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/alloc_ptrdiff_max_fail
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION FAILED$
