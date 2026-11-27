/* #10774: --interval-analysis simplifies the unused libm models, whose
 * remainder() lowers to ieee_rem; under --fixedbv that node is fixedbv. */
#include <math.h>
#include <assert.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 3)
    assert(x != 4);
  return 0;
}
// Imported from ESBMC regression/floats-regression/github_10774_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --ub-div-by-zero-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
