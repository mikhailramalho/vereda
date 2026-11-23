#include <math.h>
#include <fenv.h>
#include <limits.h>

extern void __VERIFIER_assume(int);
extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }

int main(void)
{
  double d;
  __VERIFIER_assume(!isinf(d));
  __VERIFIER_assume(!isnan(d));

  __VERIFIER_assume(d < LLONG_MAX && d > LLONG_MIN);

  double d1 = (long long) d;
  __VERIFIER_assert(trunc(d) == d1);

  return 0;
}

// Imported from ESBMC regression/floats-regression/trunc_nondet_2
// Needs: --interval-analysis
// RUN: --floatbv --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
