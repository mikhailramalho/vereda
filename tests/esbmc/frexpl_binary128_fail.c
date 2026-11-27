#include <assert.h>
#include <math.h>

int main()
{
  int e;
  long double m = frexpl(1024.0L, &e);
  assert(m == 0.5L && e == 11);
  assert(ldexpl(1.0L, 3) != 8.0L);
  return 0;
}
// Imported from ESBMC regression/esbmc/frexpl_binary128_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*ldexpl
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict FAILED
