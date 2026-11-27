#include <assert.h>
#include <complex.h>

int main()
{
  double complex z = 3.0 + 2.0 * I;

  assert(cimag(z) == 99.0);

  return 0;
}
// Imported from ESBMC regression/esbmc/complex_10
// MODE: THOROUGH
// EXPECT: ^VERIFICATION FAILED$
