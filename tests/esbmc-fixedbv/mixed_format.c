/* Mixed-format operands: TR 18037 computes in the common format without
 * double rounding. Values pinned by native execution. */
#include <assert.h>

int main(void)
{
  short _Fract mf = -0.5hr;
  _Accum mk2 = 1.25k;
  assert(mf + mk2 == 0.75k);
  assert(mk2 * mf == -0.625k);
  return 0;
}
// Imported from ESBMC regression/fixedbv/mixed_format
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
