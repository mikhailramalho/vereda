#include <assert.h>

long double __VERIFIER_nondet_ld(void);

int main(void)
{
  /* Under --32, long double is 96 bits wide, a format Bitwuzla cannot build. */
  long double x = __VERIFIER_nondet_ld();
  __VERIFIER_assume(x > 1.0L && x < 2.0L);
  long double y = x * 2.0L;
  assert(y > 2.0L && y < 4.0L);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/github_8097_long_double_32
// RUN: --solver bitwuzla -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
