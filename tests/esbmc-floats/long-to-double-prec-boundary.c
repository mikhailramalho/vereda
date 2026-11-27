/* Round-trip long long -> double -> long long across the double precision
 * boundary (2^53).  Integers above 2^53 are not all representable as double;
 * specifically 2^53+1 rounds to 2^53 under IEEE round-to-nearest-even.  After
 * the round-trip the result can therefore never equal 2^53+1. */
#include <assert.h>

long long __VERIFIER_nondet_longlong(void);

int main(void)
{
  long long x = __VERIFIER_nondet_longlong();
  __VERIFIER_assume(x >= 9007199254740992LL && x <= 9007199254740994LL);
  double d = (double)x;
  long long y = (long long)d;
  assert(y != 9007199254740993LL);
  return 0;
}
// Imported from ESBMC regression/floats/long-to-double-prec-boundary
// RUN: --solver z3 -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
