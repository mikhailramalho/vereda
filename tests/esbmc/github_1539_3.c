#include <stdint.h>

/* Casting a global's address back from an integer must yield the global,
 * even when the slicer drops every step taking its address before the cast. */
int a;
uintptr_t __VERIFIER_nondet_uintptr(void);
int main(void)
{
  uintptr_t ua = (uintptr_t)&a;
  uintptr_t x = __VERIFIER_nondet_uintptr();
  int *p = (int *)x;
  if (x == ua)
    __VERIFIER_assert(p == &a); // cast of a's address is &a
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1539_3
// EXPECT: ^VERIFICATION SUCCESSFUL$
