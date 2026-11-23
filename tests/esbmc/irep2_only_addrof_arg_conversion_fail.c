/* The store through the pointer argument writes the real sum, so a wrong
   expected value must be caught -- and caught as the assertion, not as the
   out-of-bounds the widened pointer used to report. */
#include <assert.h>

int main(void)
{
  unsigned int ux = 2;
  unsigned int uy;
  __builtin_uadd_overflow(2, ux, &uy);
  assert(uy == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_addrof_arg_conversion_fail
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
