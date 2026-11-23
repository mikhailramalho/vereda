#include <assert.h>
#include <math.h>

int main(void)
{
  volatile double big = 1e10;
  assert(asinh(-big) == -INFINITY);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9428_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
