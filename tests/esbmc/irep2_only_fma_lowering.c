#include <assert.h>
#include <math.h>

int main(void)
{
  assert(fma(2.0, 3.0, 4.0) == 10.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_fma_lowering
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
