/* #9362: a complex value with a NaN part is unequal to itself. */
#include <assert.h>
#include <math.h>

int main(void)
{
  double _Complex n = NAN;
  assert(n == n);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9362_nan_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
