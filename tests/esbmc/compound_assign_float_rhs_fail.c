#include <assert.h>
#include <fenv.h>

int main()
{
  long long x = 1LL << 53;
  fesetround(FE_UPWARD);
  x += 1.0;
  assert(x == (1LL << 53));
}
// Imported from ESBMC regression/esbmc/compound_assign_float_rhs_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
