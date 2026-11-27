#include <assert.h>
#include <math.h>

float __VERIFIER_nondet_float();

int main()
{
  float f=__VERIFIER_nondet_float();

  double d = (double)f;
  float ff = (float)d;

  assert((f == ff) || (isnan(f) && isnan(ff)));
}
// Imported from ESBMC regression/floats/Float-to-double1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
