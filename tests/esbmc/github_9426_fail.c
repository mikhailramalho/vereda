#include <assert.h>
#include <float.h>
#include <math.h>

int main(void)
{
  volatile double big = DBL_MAX, two = 2.0, minf = -INFINITY;
  double r = fma(big, two, minf);
  assert(isnan(r));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9426_fail
// Needs: --no-standard-checks
// RUN: -m64 --no-standard-checks
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
