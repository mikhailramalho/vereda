#include <assert.h>

int main()
{
  _Bool a = __VERIFIER_nondet_bool();
  _Bool b = __VERIFIER_nondet_bool();

  // ~(a ^ b) | (a & b) --> ~(a ^ b)
  assert((~(a ^ b) | (a & b)) == (~(a ^ b)));
  assert((~(a ^ b) | (b & a)) == (~(a ^ b)));
  assert(((a & b) | ~(a ^ b)) == (~(a ^ b)));
  assert(((b & a) | ~(a ^ b)) == (~(a ^ b)));

  // ~(a & b) | (a ^ b) --> ~(a & b)
  assert((~(a & b) | (b ^ a)) == (~(a & b)));
  assert((~(a & b) | (a ^ b)) == (~(a & b)));
  assert(((a ^ b) | ~(a & b)) == (~(a & b)));
  assert(((b ^ a) | ~(b & a)) == (~(a & b)));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier11_no
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
