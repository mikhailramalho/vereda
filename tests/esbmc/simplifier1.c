#include <assert.h>
#include <limits.h>
#include <math.h>

int main()
{
  _Bool x = __VERIFIER_nondet_bool();

  assert((x && x) == x);
  assert((x || x) == x);

  int n = -1;
  assert((n == n) == 1);
  assert((n < n) == 0);

  int y = __VERIFIER_nondet_int();

  assert((y == y) == 1);
  assert((y != y) == 0);
  assert((y < y) == 0);
  assert((y > y) == 0);
  assert((y <= y) == 1);
  assert((y >= y) == 1);

  _Bool a = __VERIFIER_nondet_bool();

  assert((a & a) == a);
  assert((a | a) == a);
  assert((a & 0) == 0);
  assert((0 & a) == 0);

  unsigned int u = __VERIFIER_nondet_uint();
  assert((u & 0) == 0);
  assert((u & UINT_MAX) == u);

  float f = __VERIFIER_nondet_float();
  __VERIFIER_assume(isnan(f));  
  assert(f != f); // must hold for NaN

  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier1
// EXPECT: ^VERIFICATION SUCCESSFUL$
