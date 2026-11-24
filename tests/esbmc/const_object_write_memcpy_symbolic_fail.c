#include <string.h>
const int t[2] = {1, 2};
int main()
{
  int s[2] = {7, 8};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n <= sizeof t);
  memcpy((void *)t, s, n);
  return 0;
}
// Imported from ESBMC regression/esbmc/const_object_write_memcpy_symbolic_fail
// EXPECT: ^  dereference failure: write access to const object `t'$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: write access to const object `t'$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
