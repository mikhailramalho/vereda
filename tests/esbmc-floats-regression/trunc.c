#include <math.h>

extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }

int main(void)
{
  __VERIFIER_assert(trunc(2.7) == 2.0);
  __VERIFIER_assert(trunc(-2.7) == -2.0);

  double c = trunc(-0.0);
  __VERIFIER_assert((c == -0.0) && signbit(c));

  c = trunc(-INFINITY);
  __VERIFIER_assert(isinf(INFINITY) && signbit(c));
}

// Imported from ESBMC regression/floats-regression/trunc
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
