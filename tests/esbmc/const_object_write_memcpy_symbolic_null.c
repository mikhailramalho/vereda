#include <string.h>
const int t[2] = {1, 2};
int main()
{
  int s[2] = {7, 8};
  int c = __VERIFIER_nondet_int();
  int *p = c ? (int *)t : NULL;
  __VERIFIER_assume(!c);
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n <= sizeof t);
  memcpy(p, s, n);
  return 0;
}
// Imported from ESBMC regression/esbmc/const_object_write_memcpy_symbolic_null
// RUN: --multi-property
// EXPECT: \A(?![\s\S]*FAILED[^\n]*write access to const object)
// EXPECT: FAILED[^\n]*NULL pointer on DST
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: FAILED[^\n]*NULL pointer on DST
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
