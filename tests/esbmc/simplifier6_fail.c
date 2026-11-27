#include <assert.h>

int main(void) 
{
  _Bool x = __VERIFIER_nondet_bool();
  _Bool y = __VERIFIER_nondet_bool();
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  unsigned u = __VERIFIER_nondet_uint();

  // -------------------------------------------------
  // Boolean algebra laws
  // -------------------------------------------------

  // De Morgan’s (positive)
  assert((~(x & y)) == ((~x) | (~y)));
  assert((~(x | y)) == ((~x) & (~y)));

  // De Morgan’s (negative: unrelated exprs)
  assert((~(x & y)) != ((~x) & (~y)));
  assert((~(x | y)) != ((~x) | (~y)));

  // Absorption (positive)
  assert((x & (x | y)) == x);
  assert((x | (x & y)) == x);

  // Absorption (negative)
  assert((x & y) != x);
  assert((x | y) != x);

  // Double negation (positive)
  assert((!!x) == x);

  // Double negation (negative)
  assert((!x) != x);

  // -------------------------------------------------
  // Logical De Morgan’s
  // -------------------------------------------------

  // Positive
  assert((!(a && b)) == (!a || !b));
  assert((!(a || b)) == (!a && !b));

  // Negative
  assert((!(a && b)) != (!a && !b));
  assert((!(a || b)) != (!a || !b));

  // -------------------------------------------------
  // Comparison negations
  // -------------------------------------------------

  // Positive
  assert((!(a == b)) == (a != b));
  assert((!(a < b)) == (a >= b));
  assert((!(a > b)) == (a <= b));

  // Negative
  assert((!(a == b)) != (a == b));
  assert((!(a < b)) != (a < b));

  // -------------------------------------------------
  // Shift simplifications
  // -------------------------------------------------

  // Positive
  assert((a >> 0) == a);

  // Negative
  assert((a >> 1) != a);

  // -------------------------------------------------
  // Logical absorption
  // -------------------------------------------------

  // Positive
  assert((a && (a || b)) == (_Bool)a);
  assert((a || (a && b)) == (_Bool)a);

  // Negative
  assert((a && b) != (_Bool)a); 
  assert((a || b) != (_Bool)a);

  // -------------------------------------------------
  // Unsigned relational simplification
  // -------------------------------------------------

  // Positive
  assert((u >= 0) == 1);

  // Negative
  assert((u > 0) != 0); // only false when u == 0

  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier6_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
