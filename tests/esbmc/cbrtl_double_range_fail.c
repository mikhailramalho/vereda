#include <assert.h>
#include <math.h>

int main()
{
  long double r = cbrtl(0x1p1020L);
  assert(r == 0x1p340L);
  assert(r == 0x1p341L);
  return 0;
}
// Imported from ESBMC regression/esbmc/cbrtl_double_range_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*r == 0x1p341L
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
