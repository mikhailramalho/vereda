#include <assert.h>

/* #6904: a program is free to define fma, remainder or nearbyint itself, so its
 * body is what must be verified, not the IEEE node. */
double fma(double a, double b, double c)
{
  return 42.0;
}

double remainder(double a, double b)
{
  return 43.0;
}

double nearbyint(double a)
{
  return 44.0;
}

int main(void)
{
  assert(fma(2.0, 3.0, 4.0) == 10.0);
  assert(remainder(5.0, 3.0) == -1.0);
  assert(nearbyint(2.5) == 2.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_ieee_shadowed_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
