#include <assert.h>
#include <math.h>

/* C17 F.10.7.2: remainder returns a zero with the sign of x. */
int main(void)
{
  assert(signbit(remainder(-1.0, 1.0)));
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_remainder_exact
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
