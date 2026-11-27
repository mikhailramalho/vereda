/* Two *different* unresolvable locations must stay free to hold different
 * values: a fix for #5369 that shares one free value across every unresolved
 * read alike, rather than keying on the location, turns this SUCCESSFUL. */
#include <assert.h>

unsigned long __VERIFIER_nondet_ulong(void);

int main(void)
{
  int *p = (int *)__VERIFIER_nondet_ulong();
  int *q = (int *)__VERIFIER_nondet_ulong();
  int a = *p;
  int b = *q;
  assert(a == b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5369_scalar_fail
// RUN: --ub-pointer-check=false
// EXPECT: ^VERIFICATION FAILED$
