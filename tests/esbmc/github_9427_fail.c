#include <assert.h>
#include <math.h>

int main(void)
{
  volatile double x = -0.0;
  double r = atan(x);
  assert(!signbit(r));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9427_fail
// RUN: -m64 --ub-pointer-check=false --ub-div-by-zero-check=false --vla-size-check=false
// EXPECT: ^VERIFICATION FAILED$
