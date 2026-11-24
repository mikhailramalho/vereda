// Floats computed from constants fold, but -0.0 and +0.0 stay apart: they
// compare equal, yet 1/x, copysign and signbit tell them apart. The inputs
// are in a global so that clang and InstCombine leave the arithmetic to
// Vereda; float_fold_signed_zero_false.c checks that the fold keeps the sign.
// RUN: --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern void __VERIFIER_assert(int);
double in[] = {0.0, 1.0, 1e-300};
int main(void) {
  double pz = in[0] * in[1], nz = -in[0], tiny = -in[2] * in[2]; // underflows to -0
  __VERIFIER_assert(pz == nz && nz == tiny && !(pz < nz) && !(pz > nz));
  __VERIFIER_assert(1 / pz == INFINITY && 1 / nz == -INFINITY && 1 / tiny == -INFINITY);
  __VERIFIER_assert(!signbit(pz) && signbit(nz) && signbit(tiny));
  __VERIFIER_assert(copysign(in[1], nz) == -1.0 && copysign(in[1], pz) == 1.0);
  __VERIFIER_assert(signbit(nz + nz) && !signbit(nz + pz) && !signbit(pz - pz));
  __VERIFIER_assert(signbit(fmin(nz, pz)) && !signbit(fmin(pz, nz))); // the first of equals
  __VERIFIER_assert(signbit((float)nz) && signbit(sqrt(nz)) && !signbit(fabs(nz)));
  double s = 0;
  for (int i = 0; i < 2; ++i) // a join of -0 and +0 is neither
    s = i ? nz : pz;
  __VERIFIER_assert(signbit(s));
  return 0;
}
