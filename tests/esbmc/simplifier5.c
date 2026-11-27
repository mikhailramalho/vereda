#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();

  // Existing basic tests
  assert((x && !x) == 0);
  assert(((x && a) && (x && b)) == (x && (a && b)));
  assert(((x || a) || (x || b)) == (x || (a || b)));

  // Swapped operand tests
  assert(((a && x) && (b && x)) == (x && (a && b)));
  assert(((a || x) || (b || x)) == (x || (a || b)));

  // Nested cases
  assert(((x && a) && ((x && b) && (x && c))) == (x && (a && (b && c))));
  assert(((x || a) || ((x || b) || (x || c))) == (x || (a || (b || c))));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier5
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
