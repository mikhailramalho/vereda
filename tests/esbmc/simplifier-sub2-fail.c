#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int c1 = 1, c2 = 2;

  // x - (-y) -> x + y
  assert((x - (-y)) == (x + y));
  assert((x + y) == (x - (-y)));

  // (-x) - y -> -(x + y)
  assert(((-x) - y) == -(x + y));
  assert(-(x + y) == ((-x) - y));

  // (x - c1) - c2 → x - (c1 + c2)
  assert(((x - c1) - c2) == (x - (c1 + c2)));
  assert((x - (c1 + c2)) == ((x - c1) - c2));
  
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-sub2-fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
