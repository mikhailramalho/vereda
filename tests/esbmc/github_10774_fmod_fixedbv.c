#include <math.h>
int main()
{
  double r = fmod(5.0, 3.0);
  __VERIFIER_assert(r == 2.0); // fmod(5,3) == 2
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10774_fmod_fixedbv
// EXPECT: ^VERIFICATION SUCCESSFUL$
// RUN: --no-div-by-zero-check
