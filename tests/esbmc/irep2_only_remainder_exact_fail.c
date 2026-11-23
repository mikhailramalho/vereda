#include <assert.h>
#include <math.h>

int main(void)
{
  assert(!signbit(remainder(-1.0, 1.0)));
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_remainder_exact_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
