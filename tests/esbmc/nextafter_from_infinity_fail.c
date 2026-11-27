#include <assert.h>
#include <float.h>
#include <math.h>

int main()
{
  double r = nextafter(INFINITY, 0.0);
  assert(r == DBL_MAX);
  assert(isinf(r));
  return 0;
}
// Imported from ESBMC regression/esbmc/nextafter_from_infinity_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*isinf\(r\)
// Vereda: unmet EXPECT: FAILED\s+\[main.assertion.2\].*isinf\(r\)
// MODE: FUTURE
// Vereda: verdict FAILED
