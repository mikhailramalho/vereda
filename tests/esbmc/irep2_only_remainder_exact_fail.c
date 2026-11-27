#include <assert.h>
#include <math.h>

int main(void)
{
  assert(!signbit(remainder(-1.0, 1.0)));
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_remainder_exact_fail
// EXPECT: ^VERIFICATION FAILED$
