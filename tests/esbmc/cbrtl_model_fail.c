#include <assert.h>
#include <math.h>

int main()
{
  long double r = cbrtl(2.0L);
  assert(r > 1.2599L && r < 1.26L);
  assert(r == 1.25L);
  return 0;
}
// Imported from ESBMC regression/esbmc/cbrtl_model_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*r == 1.25
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict FAILED
