#include <assert.h>
#include <math.h>

int main()
{
  assert(ilogb(8.0) == 3);
  assert(logb(0.75) == 0.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/ilogb_logb_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*logb
// Vereda: unmet EXPECT: FAILED\s+\[main.assertion.2\].*logb
// MODE: FUTURE
// Vereda: verdict FAILED
