// Vereda rounds every float operation to nearest, ties to even, and keeps
// no rounding mode (fesetround has no body), so constants fold under that
// mode too: the folded quotient, sum and conversion here have the same bits
// as the ones the solver computes from a value it only knows by assumption.
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <fenv.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned long __VERIFIER_nondet_ulong(void);
double in[] = {1.0, 3.0, 0x1p53, 0x1.0000000000001p0};
static unsigned long bits(double d) {
  unsigned long u;
  memcpy(&u, &d, sizeof u);
  return u;
}
int main(void) {
  fesetround(FE_UPWARD);
  unsigned long u = __VERIFIER_nondet_ulong();
  __VERIFIER_assume(u == bits(in[0]));
  double one;
  memcpy(&one, &u, sizeof one);
  __VERIFIER_assert(bits(in[0] / in[1]) == bits(one / in[1]));
  __VERIFIER_assert(bits(in[2] + in[0]) == bits(in[2] + one)); // a tie, to even
  __VERIFIER_assert((float)in[3] == (float)(in[3] * one));
  __VERIFIER_assert(in[0] / in[1] == 0x1.5555555555555p-2 && in[2] + in[0] == 0x1p53);
  return 0;
}
