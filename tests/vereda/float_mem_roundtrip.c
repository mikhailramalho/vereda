// Storing a float or double to memory and loading it back cancels the
// fromBits(toBits(v, ty), ty) round-trip while preserving IEEE-754 value
// semantics for normal numbers, signed zeros (+0.0 and -0.0), and NaNs.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>

extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  volatile float vf = __VERIFIER_nondet_float();
  float lf = vf;
  __VERIFIER_assert(lf == vf || (isnan(lf) && isnan(vf)));

  // Signed zero: -0.0 and +0.0 stay distinct across memory.
  volatile float zpos = 0.0f;
  volatile float zneg = -0.0f;
  float lp = zpos, ln = zneg;
  __VERIFIER_assert(lp == 0.0f && !signbit(lp));
  __VERIFIER_assert(ln == 0.0f && signbit(ln));

  // Symbolic signed zero preserves its sign bit across memory.
  float sf = __VERIFIER_nondet_float();
  __VERIFIER_assume(sf == 0.0f && signbit(sf));
  volatile float vsf = sf;
  float lsf = vsf;
  __VERIFIER_assert(lsf == 0.0f && signbit(lsf));

  // Constant and symbolic NaN stay NaN across memory.
  volatile double vnan = __builtin_nan("");
  double lnan = vnan;
  __VERIFIER_assert(isnan(lnan) && lnan != lnan);

  double sd = __VERIFIER_nondet_double();
  __VERIFIER_assume(isnan(sd));
  volatile double vsd = sd;
  double lsd = vsd;
  __VERIFIER_assert(isnan(lsd) && lsd != lsd);
  return 0;
}
