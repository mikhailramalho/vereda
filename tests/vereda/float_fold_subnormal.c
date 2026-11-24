// Folded arithmetic on constants keeps subnormals (no flush to zero),
// rounds ties to even down to zero, and overflows to infinity, in
// operations and in conversions between widths and from integers.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern void __VERIFIER_assert(int);
double in[] = {0x1p-1022, 0x1p-1074, 0x1.fffffffffffffp1023, 2.0, 1e39, 1e-46, 0x1.8p-149};
long big[] = {16777217, 9007199254740993L, -9007199254740995L};
int main(void) {
  double sub = in[0] / in[3];
  __VERIFIER_assert(sub == 0x1p-1023 && fpclassify(sub) == FP_SUBNORMAL && sub * in[3] == in[0]);
  __VERIFIER_assert(in[1] / in[3] == 0 && !signbit(in[1] / in[3]));  // a tie, to even zero
  __VERIFIER_assert(in[1] * 1.5 == 2 * in[1] && in[1] - in[1] == 0); // 1.5 ulp rounds up
  __VERIFIER_assert(in[2] * in[3] == INFINITY && -in[2] * in[3] == -INFINITY);
  __VERIFIER_assert(in[2] + in[2] - in[2] == INFINITY && isinf(in[2] * in[2]));
  __VERIFIER_assert((float)in[4] == INFINITY && (float)-in[4] == -INFINITY);
  __VERIFIER_assert((float)in[5] == 0 && (float)in[6] == 0x1p-148f); // a tie to even subnormal
  __VERIFIER_assert(fpclassify((float)in[0]) == FP_ZERO && fpclassify((float)in[6]) == FP_SUBNORMAL);
  __VERIFIER_assert((float)big[0] == 16777216.0f && (double)big[1] == 0x1p53);
  __VERIFIER_assert((double)big[2] == -0x1.0000000000002p53);
  return 0;
}
