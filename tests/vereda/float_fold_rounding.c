// After fesetround(FE_UPWARD) every float operation rounds up, one on
// constants as well as one on a value known only by assumption, so the
// quotient, sum and conversion here have the same bits either way, those a
// native run built with -frounding-math gives (#301).
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
  __VERIFIER_assert(bits(in[2] + in[0]) == bits(in[2] + one)); // a tie, up
  __VERIFIER_assert((float)in[3] == (float)(in[3] * one));
  __VERIFIER_assert(in[0] / in[1] == 0x1.5555555555556p-2 &&
                    in[2] + in[0] == 0x1.0000000000001p53);
  return 0;
}
