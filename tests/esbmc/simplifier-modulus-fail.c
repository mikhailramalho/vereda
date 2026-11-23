#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x % x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-modulus-fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
