// The failing dual of ../isnormal_infinity (#7320).
#include <assert.h>
#include <math.h>

int main(void)
{
  double inf = 1.0 / 0.0;
  assert(__builtin_isnormal(inf));
  return 0;
}
// Imported from ESBMC regression/floats/isnormal_infinity_fail
// EXPECT: ^VERIFICATION FAILED$
