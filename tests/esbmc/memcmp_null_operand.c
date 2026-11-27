#include <assert.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint();

int main()
{
  char a[4] = {1, 2, 3, 4}, b[4] = {1, 2, 3, 5};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 4);
  // A null operand is fine when no byte is read (C2y 7.26.1, N3322).
  char *p = n ? a : 0;
  int r = memcmp(b, p, n);
  assert(n < 4 ? r == 0 : r > 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/memcmp_null_operand
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
