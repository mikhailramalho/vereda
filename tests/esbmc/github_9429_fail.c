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
// RUN: --64 --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
