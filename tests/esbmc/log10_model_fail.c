#include <assert.h>
#include <math.h>

int main()
{
  double r = log10(2.0);
  assert(r > 0.30102 && r < 0.30103);
  assert(log10(1000.0) != 3.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/log10_model_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*log10\(1000.0\) != 3.0
// Vereda: unmet EXPECT: FAILED\s+\[main.assertion.2\].*log10\(1000.0\) != 3.0
// MODE: FUTURE
// Vereda: verdict FAILED
