// #677: the write through a masked pointer must land where the mask put it.
#include <stdint.h>

char buf[16];

int main(void)
{
  char *p = (char *)((uintptr_t)(buf + 1) & ~(uintptr_t)7);
  *p = 42;
  __VERIFIER_assert(
    buf[(uintptr_t)p - (uintptr_t)buf] == 42); // read back the write
  return 0;
}
// Imported from ESBMC regression/esbmc/github_677_mask_safe
// EXPECT: ^VERIFICATION SUCCESSFUL$
