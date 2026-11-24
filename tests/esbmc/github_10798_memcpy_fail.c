#include <stdint.h>
#include <string.h>

/* A pointer whose bytes are copied from a nondet integer can name any object,
 * b included, with or without slicing (#10798). */
uintptr_t __VERIFIER_nondet_uintptr(void);
int main(void)
{
  int a = 1, b = 2;
  int *q = &a;
  (void)q;
  uintptr_t i = __VERIFIER_nondet_uintptr();
  int *p;
  memcpy(&p, &i, sizeof p);
  __VERIFIER_assert(
    __ESBMC_POINTER_OBJECT(p) <= 1 || __ESBMC_same_object(p, &b)
); // p names null, invalid or b
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10798_memcpy_fail
// EXPECT: ^VERIFICATION FAILED$
