// KNOWNBUG since #7964: the inductive step proved this only by pinning r1 to
// its loop-entry target; a sound proof needs the loop-head fixpoint.
#include <assert.h>
int __VERIFIER_nondet_int();
int x[2], y[2];
int main()
{
  x[0] = 1;
  x[1] = 2;
  y[0] = 3;
  y[1] = 4;
  int z = __VERIFIER_nondet_int(); // input
  int *r1 = &x[1];
  int *r2 = &y[1];
  while(z > 0)
  {
    int *tmp = r1;
    r1 = r2;
    r2 = tmp;
    z = z - 1;
  }
  assert(*r1 == 2 || *r1 == 4);
}
// Imported from ESBMC regression/incremental-smt/incremental-24
// Needs: --k-induction
// RUN: --k-induction
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: KNOWNBUG
// Vereda: verdict none
