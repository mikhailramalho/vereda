/* #9221: no address is read, but only two objects of 2^62 bytes fit in a 64-bit
 * address space, one on each side of 2^63, so the third allocation cannot
 * succeed. */
#include <stdlib.h>
int main(void)
{
  for (int i = 0; i < 3; i++)
  {
    char *p = malloc(1UL << 62);
    __VERIFIER_assert(__ESBMC_get_object_size(p) == 1UL << 62); // allocated
  }
  __VERIFIER_assert(0); // three objects of 2^62 bytes fit
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9221_exhaust
// Needs: __ESBMC_get_object_size
// RUN: -m64 --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
