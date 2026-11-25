/* Round-trip int -> float -> int across the float precision boundary (2^24).
 * Integers above 2^24 are not all representable as float; specifically 2^24+1
 * rounds to 2^24 under IEEE round-to-nearest-even.  After the round-trip the
 * result can therefore never equal 2^24+1. */
#include <assert.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 16777216 && i <= 16777218);
  float f = (float)i;
  int j = (int)f;
  assert(j != 16777217);
  return 0;
}
// Imported from ESBMC regression/floats/int-to-float-prec-boundary
// RUN: --solver z3 -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
