#include <stdint.h>

/* #9273: a cast that lands on a still names a's address. */
int a;
uintptr_t __VERIFIER_nondet_uintptr(void);
_Bool __VERIFIER_nondet_bool(void);
int main(void)
{
  uintptr_t n = __VERIFIER_nondet_uintptr();
  int *p = (int *)n;
  uintptr_t ua = 1;
  if (__VERIFIER_nondet_bool())
    ua = (uintptr_t)&a;
  __VERIFIER_assert(p != &a || n == (uintptr_t)&a); // p is a only at a's address
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9273
// Needs: --smt-symex-guard
// RUN: --smt-symex-guard
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
