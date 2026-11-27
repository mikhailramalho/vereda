#include <assert.h>

int main()
{
  int c = __VERIFIER_nondet_int();
  int d = __VERIFIER_nondet_int();
  int e = __VERIFIER_nondet_int();

  // Pattern: d + c == d + e -> c == e

  // Variation 1: (d + c) == (d + e)
  assert(((d + c) == (d + e)) == (c == e));

  // Variation 2: (d + c) == (e + d) [commutative on right]
  assert(((d + c) == (e + d)) == (c == e));

  // Variation 3: (c + d) == (d + e) [commutative on left]
  assert(((c + d) == (d + e)) == (c == e));

  // Variation 4: (c + d) == (e + d) [both commutative]
  assert(((c + d) == (e + d)) == (c == e));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier12-overflow
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
