#include <stdint.h>

/* The integer cast can land on a global whose address first appears in the
 * formula after the cast. */
int a;
uintptr_t __VERIFIER_nondet_uintptr(void);
int main(void)
{
  uintptr_t ua = (uintptr_t)&a;
  int *p = (int *)__VERIFIER_nondet_uintptr();
  __VERIFIER_assert(p != &a || ua == 0); // p may point to a
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1539_3_fail
// EXPECT: ^VERIFICATION FAILED$
