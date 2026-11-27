#include <assert.h>

int main()
{
  float c = __VERIFIER_nondet_float();
  float d = __VERIFIER_nondet_float();
  float e = __VERIFIER_nondet_float();

  // Pattern: d + c == d + e -> c == e

  // Variation 1: (d + c) == (d + e)
  assert(((d + c) == (d + e)) == (c == e));

  // Variation 2: (d + c) == (e + d) [commutative on right]
  assert(((d + c) == (e + d)) == (c == e));

  // Variation 3: (c + d) == (d + e) [commutative on left]
  assert(((c + d) == (d + e)) == (c == e));

  // Variation 4: (c + d) == (e + d) [both commutative]
  assert(((c + d) == (e + d)) == (c == e));

  float b = __VERIFIER_nondet_float();

  //  d + b == c + d  ->  b == c
  assert(((d + b) == (c + d)) == (b == c));

  float a = __VERIFIER_nondet_float();

  // a + c == c + d  ->  a == d
  assert(((a + c) == (c + d)) == (a == d));

  // a + d == c + d  ->  a == c
  assert(((a + d) == (c + d)) == (a == c));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier13
// EXPECT: ^VERIFICATION FAILED$
