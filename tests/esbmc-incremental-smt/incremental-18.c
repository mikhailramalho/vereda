// KNOWNBUG since #7964: the inductive step proved this only by pinning r1 to
// its loop-entry target; a sound proof needs the loop-head fixpoint.

#include <assert.h>
int __VERIFIER_nondet_int();
int main()
{
  int x = __VERIFIER_nondet_int(); // input
  int y = __VERIFIER_nondet_int(); // input
  int z = __VERIFIER_nondet_int(); // input
  int *r1 = &x;
  int *r2 = &y;
  while(z > 0)
  {
    int *tmp = r1;
    r1 = r2;
    r2 = tmp;
    z = z - 1;
  }
  assert(*r1 == x || *r1 == y);
}
// Imported from ESBMC regression/incremental-smt/incremental-18
// Needs: --k-induction
// RUN: --k-induction
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: KNOWNBUG
// Vereda: verdict none
