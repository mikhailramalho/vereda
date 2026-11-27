#include <assert.h>
#include <math.h>
double __VERIFIER_nondet_double(void);
static double V(double c){ double x=__VERIFIER_nondet_double(); __VERIFIER_assume(x==c); return x; }
int main(void)
{
  assert(remainder(V(6.0), V(4.0)) == -2.0);
  assert(fmod(V(6.0), V(4.0)) == 2.0);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/native_fp_rem
// RUN: --solver bitwuzla
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
