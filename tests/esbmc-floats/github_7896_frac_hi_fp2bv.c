/* github #7896: binary16 has 10 significand bits, so the gap above 1 is
 * 2^-10 and 1 + 2^-11 rounds back to 1. Pins the significand from above.
 * --fp2bv lowers to bit-vectors, so the solver never sees an FP sort it could
 * reject instead of answering. */
#include <assert.h>

_Float16 __VERIFIER_nondet_h(void);

int main(void)
{
  _Float16 x = __VERIFIER_nondet_h();
  __VERIFIER_assume((float)x == 1.0f);
  _Float16 y = x + (_Float16)0x1p-11f;
  assert(y != x);
  return 0;
}
// Imported from ESBMC regression/floats/github_7896_frac_hi_fp2bv
// RUN: --fp2bv --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
