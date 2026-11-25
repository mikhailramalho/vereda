// A loop on floats computed from constants alone needs no --unwind: each
// guard folds, as an integer loop's does (loop_const.c). Fused multiply-add
// on constants rounds once, as the encoder fuses it, and square roots fold
// correctly rounded.
// EXPECT: ^\[PROGRESS\] Unwinding loop 0 iteration 30
// EXPECT-NOT: iteration 31
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern void __VERIFIER_assert(int);
double in[] = {1.0, 1e-9, 0x1.0000002p0, -0x1.0000004p0, 2.0, 2.25};
float fin[] = {2.0f, 0x1.fffffep127f};
int main(void) {
  double x = in[0];
  while (x > in[1])
    x *= 0.5;
  __VERIFIER_assert(x == 0x1p-30);
  __VERIFIER_assert(fma(in[2], in[2], in[3]) == 0x1p-54);
  __VERIFIER_assert(in[2] * in[2] + in[3] == 0x1p-54); // contracted, as clang does by default
  __VERIFIER_assert(sqrt(in[4]) == 0x1.6a09e667f3bcdp0 && sqrt(in[5]) == 1.5);
  __VERIFIER_assert(sqrtf(fin[0]) == 0x1.6a09e6p0f && sqrtf(fin[1]) == 0x1.fffffep63f);
  return 0;
}
