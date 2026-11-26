#include <assert.h>
#include <math.h>

int main(void)
{
  volatile double n = -0.0, p = 0.0;
  assert(signbit(atan(n)) && atan(n) == 0.0);
  assert(!signbit(atan(p)) && atan(p) == 0.0);
  assert(atan(1.0) > 0.78 && atan(1.0) < 0.79);
  assert(atan(-1.0) < -0.78 && atan(-1.0) > -0.79);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9427
// Needs: --no-standard-checks
// RUN: -m64 --no-standard-checks
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
