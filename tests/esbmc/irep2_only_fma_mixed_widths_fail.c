#include <assert.h>

long double fmal(double, double, double);

int main(void)
{
  assert(fmal(2.0, 3.0, 4.0) == 10.0L);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_fma_mixed_widths_fail
// Needs: --clang-c-irep2-adjust-only REQUIRES wide_long_double
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
