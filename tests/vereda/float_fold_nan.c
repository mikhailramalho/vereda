// Comparisons and class tests of a constant NaN fold as the encoding has
// them: unordered, unequal to itself, true as a condition, skipped by fmin
// and fmax. Operations that give a NaN do not fold, since its bits are the
// encoding's (float_fold_nan_payload.c), but it is still a NaN.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern void __VERIFIER_assert(int);
double in[] = {0.0, 1.0, NAN, INFINITY};
int main(void) {
  double nan = in[2], one = in[1], inf = in[3];
  __VERIFIER_assert(nan != nan && !(nan == nan) && !(nan < one) && !(nan >= one));
  __VERIFIER_assert(isunordered(nan, one) && !islessgreater(nan, one) && isnan(nan));
  __VERIFIER_assert(fpclassify(nan) == FP_NAN && !isinf(nan) && !isnormal(nan));
  __VERIFIER_assert(fmin(nan, one) == one && fmax(one, nan) == one && (nan ? 1 : 0));
  double zero = in[0];
  __VERIFIER_assert(isnan(zero / zero) && isnan(inf - inf) && isnan(inf * zero));
  __VERIFIER_assert(isnan(nan + one) && isnan(-nan) && isnan(fabs(nan)) && isnan(sqrt(-one)));
  __VERIFIER_assert(isnan((float)nan) && isnan(fma(nan, one, one)));
  return 0;
}
