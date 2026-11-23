#include <assert.h>

int remainder(int, int);

int main(void)
{
  assert(remainder(7, 3) == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_remainder_user_int_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
