/* The pre-fix reading: the offsets were compared unsigned, so b-1 sorted above
   b. Asserting that must now fail. */
#include <assert.h>

int main(void)
{
  char a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  char *b = a;
  char *below = b - 1;

  assert(below >= b);
  return 0;
}
// Imported from ESBMC regression/esbmc/ptr_rel_below_base_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
