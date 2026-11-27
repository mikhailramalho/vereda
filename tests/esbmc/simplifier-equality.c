#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  const int c1 = 1, c2 = 2, c = 3;

  // (x + c1) == c2 -> x == (c2 - c1)
  assert(((x + c1) == c2) == (x == (c2 - c1)));
  assert((x == (c2 - c1)) == ((x + c1) == c2));
  
  // (x - c1) == c2 -> x == (c2 + c1)
  assert(((x - c1) == c2) == (x == (c2 + c1)));
  assert((x == (c2 + c1)) == ((x - c1) == c2));

  // (x * c) == 0 -> x == 0 (when c != 0)
  assert(((x * c) == 0) == (x == 0));
  assert((x == 0) == ((x * c) == 0));
  
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-equality
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
