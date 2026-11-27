#include <assert.h>
#include <math.h>

int main(void)
{
  volatile double x = -0.0;
  double r = atanh(x);
  assert(!signbit(r));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9429_fail
// Needs: --no-standard-checks
// RUN: -m64 --no-standard-checks --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
