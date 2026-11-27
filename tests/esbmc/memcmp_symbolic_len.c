#include <string.h>

int main()
{
  char a[4] = "abc";
  char b[4] = "abd";
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 4);
  int r = memcmp(a, b, n);
  __VERIFIER_assert(n < 3 ? r == 0 : r < 0); // first difference is at byte 2
  return 0;
}
// Imported from ESBMC regression/esbmc/memcmp_symbolic_len
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
