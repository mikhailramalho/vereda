/* #9350: a float sum whose first operand is a constant was printed in the
 * counterexample as that constant instead of the sum. */
#include <assert.h>
float __VERIFIER_nondet_float(void);
double __VERIFIER_nondet_double(void);

int main(void)
{
  float h = __VERIFIER_nondet_float();
  __VERIFIER_assume(h >= 1.0f && h <= 1.0f);
  double g = __VERIFIER_nondet_double();
  __VERIFIER_assume(g >= 2.0 && g <= 2.0);
  float a = 0.25f + h;
  float s = 0.25f - h;
  float m = 0.25f * h;
  float d = 0.5f / h;
  double e = 0.25 + g;
  assert(a + s + m + d + e < 0.0);
  return 0;
}
// Imported from ESBMC regression/floats/github_9350
// EXPECT: ^  a = 1\.250000e\+0f; //
// EXPECT: ^  s = -7\.500000e-1f; //
// EXPECT: ^  m = 2\.500000e-1f; //
// EXPECT: ^  d = 5\.000000e-1f; //
// EXPECT: ^  e = 2\.250000e\+0; //
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  a = 1\.250000e\+0f \(
// MODE: FUTURE
// Vereda: verdict FAILED
