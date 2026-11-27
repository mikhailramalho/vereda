#include <math.h>
int main()
{
  double r = fmod(5.0, 3.0);
  __VERIFIER_assert(r == 1.0); // fmod(5,3) is 2, not 1
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10774_fmod_fixedbv_fail
// EXPECT: ^VERIFICATION FAILED$
// RUN: --ub-div-by-zero-check=false
