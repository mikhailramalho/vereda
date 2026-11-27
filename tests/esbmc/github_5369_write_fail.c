/* A write between two reads of an unresolvable location must be observed: a
 * fix for #5369 that shares the free value keyed on the pointer alone misses
 * it, because a write through an unresolved pointer leaves the pointer's own
 * SSA value unchanged. */
#include <assert.h>

unsigned long __VERIFIER_nondet_ulong(void);

int main(void)
{
  int *p = (int *)__VERIFIER_nondet_ulong();
  int a = *p;
  *p = a + 1;
  int b = *p;
  assert(a == b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5369_write_fail
// RUN: --ub-pointer-check=false --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
