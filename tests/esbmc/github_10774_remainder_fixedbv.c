#include <math.h>
int main()
{
  int q;
  __VERIFIER_assert(remainder(5.0, 3.0) == -1.0); // remainder(5,3) == -1
  __VERIFIER_assert(remquo(5.0, 3.0, &q) == -1.0 && q == 2); // remquo(5,3) == -1
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10774_remainder_fixedbv
// EXPECT: ^VERIFICATION SUCCESSFUL$
// RUN: --ub-div-by-zero-check=false
