/* The sign is carried through: -INFINITY is -1, not 1. */
#include <assert.h>
#include <math.h>

int main(void)
{
  assert(__builtin_isinf_sign(-INFINITY) == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_isinf_sign_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
