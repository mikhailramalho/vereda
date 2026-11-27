#include <assert.h>

double fma(double, double);
double nearbyint(void);
double remainder(double);

int main(void)
{
  assert(fma(2.0, 3.0) == 5.0 && nearbyint() == 0.0 && remainder(7.0) == 7.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_ieee_arity_decline_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
