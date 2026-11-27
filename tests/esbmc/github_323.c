#include <assert.h>

#if 0
  #define TYPE __CPROVER_bitvector [10]
#else
  #define TYPE 
#endif

int main() {
  _ExtInt(10) x = __VERIFIER_nondet_float();
  _ExtInt(10) y = __VERIFIER_nondet_int();
  _ExtInt(10) z = x + y;
  z = x - y;
  z = x * y;
  z = x / y;
  z = x % y;
  z = x | y;
  z = x & y;
  z = x ^ y;
  z = x >> y;
  z = x << y;
  assert(z == x + y);
}
// Imported from ESBMC regression/esbmc/github_323
// EXPECT: ^VERIFICATION FAILED$
