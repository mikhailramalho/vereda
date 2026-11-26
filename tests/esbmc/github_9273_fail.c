#include <stdint.h>

/* #9273: under --smt-during-symex the cast is converted at the branch, before
 * &a first appears, and must still be able to land on a. */
int a;
uintptr_t __VERIFIER_nondet_uintptr(void);
_Bool __VERIFIER_nondet_bool(void);
int main(void)
{
  int *p = (int *)__VERIFIER_nondet_uintptr();
  uintptr_t ua = 1;
  if (__VERIFIER_nondet_bool())
    ua = (uintptr_t)&a;
  __VERIFIER_assert(p != &a || ua == 0 || ua == 1); // p may point to a
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9273_fail
// RUN:
// EXPECT: ^VERIFICATION FAILED$
