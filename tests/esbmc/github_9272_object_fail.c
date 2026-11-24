#include <stdint.h>

/* Only a sliced step takes a's address, yet the cast pointer can name a, and
 * POINTER_OBJECT tells a apart from b, as it does under --no-slice. */
uintptr_t __VERIFIER_nondet_uintptr(void);
int main(void)
{
  int a = 1, b = 2;
  int *q = &a;
  (void)q;
  uintptr_t x = __VERIFIER_nondet_uintptr();
  int *p = (int *)x;
  __VERIFIER_assert(
    __ESBMC_POINTER_OBJECT(p) <= 1 || __ESBMC_same_object(p, &b)
); // p names null, an invalid object or b
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9272_object_fail
// Needs: __ESBMC_POINTER_OBJECT, __ESBMC_same_object
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
