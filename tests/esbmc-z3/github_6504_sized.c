/* Positive control for github_6504: the same shape with the allocation
 * constrained to hold the object being read, so the bounds check must
 * discharge. Guards against pinning the false negative's opposite -- a build
 * that reports every nondet-sized heap access as out of bounds.
 */
#include <stdlib.h>

int main()
{
  unsigned long n;
  __VERIFIER_assume(n >= sizeof(int));
  int *p = (int *)malloc(n);
  __VERIFIER_assume(p != 0);
  int snap = *p;
  *p = *p + 5;
  __VERIFIER_assert(*p == snap + 5); // post
  return 0;
}
// Imported from ESBMC regression/z3/github_6504_sized
// RUN: --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
