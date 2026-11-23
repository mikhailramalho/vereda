#include <assert.h>
#include <fenv.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i <= 100);
  int d = i;
  d /= 2.5;
  assert(d == i * 2 / 5);

  long long x = 1LL << 53;
  fesetround(FE_UPWARD);
  x += 1.0;
  assert(x == (1LL << 53) + 2);
}
// Imported from ESBMC regression/esbmc/compound_assign_float_rhs
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
