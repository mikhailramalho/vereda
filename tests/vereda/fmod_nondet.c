// fmodf on any floats: NaN when x is infinite, y is zero or either is NaN;
// otherwise smaller in magnitude than y and no larger than x, with x's sign
// (zeros included), which SMT's round-to-nearest fp.rem alone breaks.
// Natively too, as frem goes through camada's bit-vector encoding there:
// on unconstrained operands Bitwuzla's own fp.rem took over 120 s.
// RUN:
// RUN: --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assert(int);
int main(void) {
  float x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  float r = fmodf(x, y);
  if (isnan(x) || isnan(y) || isinf(x) || y == 0.0f) {
    __VERIFIER_assert(isnan(r));
  } else {
    __VERIFIER_assert(fabsf(r) < fabsf(y));
    __VERIFIER_assert(fabsf(r) <= fabsf(x));
    __VERIFIER_assert(!signbit(r) == !signbit(x));
  }
  return 0;
}
