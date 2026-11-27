#include <assert.h>
#include <math.h>

int main()
{
  double r = pow(101.0, 0.5);
  assert(r > 10.1);
  return 0;
}
// Imported from ESBMC regression/floats/pow_fractional_exponent_fail
// EXPECT: ^VERIFICATION FAILED$
