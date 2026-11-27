#include<math.h>

int main()
{
  double d, q, r;
  __VERIFIER_assume(isfinite(q));
  d=q;
  r=d+0;
  assert(r==d);
}
// Imported from ESBMC regression/floats/Float8
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
