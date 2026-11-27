#include <stdint.h>

/* The integer cast of a local's address yields the local, so the assertion
 * fails, even when the slicer drops every step taking its address before the
 * cast (#9272). */
uintptr_t __VERIFIER_nondet_uintptr(void);
int main(void)
{
  int a;
  uintptr_t ua = (uintptr_t)&a;
  uintptr_t x = __VERIFIER_nondet_uintptr();
  int *p = (int *)x;
  if (x == ua)
    __VERIFIER_assert(p != &a); // cast of a's address is not &a
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9272_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
