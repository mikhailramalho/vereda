#include <string.h>

int main()
{
  char a[4] = "abc";
  char b[4] = "abd";
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 5);
  return memcmp(a, b, n) != 0;
}
// Imported from ESBMC regression/esbmc/memcmp_symbolic_len_fail
// EXPECT: ^  FAILED .*dereference failure: memcmp length exceeds object bounds$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED .*dereference failure: memcmp length exceeds object bounds$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
