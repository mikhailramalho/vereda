#include <fenv.h>
#include <math.h>

extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }

int main(void)
{
  fesetround(FE_TONEAREST);
  __VERIFIER_assert(lrint(2.3) == 2);
  __VERIFIER_assert(lrint(2.5) == 2);
  __VERIFIER_assert(lrint(3.5) == 4);
  __VERIFIER_assert(lrint(-2.3) == -2);
  __VERIFIER_assert(lrint(-2.5) == -2);
  __VERIFIER_assert(lrint(-3.5) == -4);

  fesetround(FE_DOWNWARD);		
  __VERIFIER_assert(lrint(2.3) == 2);
  __VERIFIER_assert(lrint(2.5) == 2);
  __VERIFIER_assert(lrint(3.5) == 3);
  __VERIFIER_assert(lrint(-2.3) == -3);
  __VERIFIER_assert(lrint(-2.5) == -3);
  __VERIFIER_assert(lrint(-3.5) == -4);

  __VERIFIER_assert(!lrint(-0.0));
}

// Imported from ESBMC regression/floats-regression/lrint
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
