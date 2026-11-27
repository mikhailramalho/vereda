/* #9785: under --smt-during-symex the guard below is asked inside a push/pop,
 * which is where the address-space stores are first asserted. After the pop
 * they must be asserted again, or a heap pointer and &x / &y may share an
 * address. */
#include <stdint.h>
#include <stdlib.h>
_Bool __VERIFIER_nondet_bool(void);
int x, y;
int main(void)
{
  int *p = __VERIFIER_nondet_bool() ? &x : &y;
  int *h = malloc(8);
  if (!h)
    return 0;
  if ((uintptr_t)p != (uintptr_t)h)
    __VERIFIER_assert(0); // the addresses compare unequal
  free(h);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9785_push_pop_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
