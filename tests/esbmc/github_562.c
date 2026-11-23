#include <assert.h>
extern unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  /* narrowing: long double (128-bit fixedbv) -> unsigned long (64) */
  unsigned long a = 1234567;
  long double ua = a;
  assert((unsigned long)ua == 1234567);

  /* truncation toward zero must still truncate */
  long double frac = 3.75L;
  assert((int)frac == 3);

  /* widening: float (32-bit fixedbv) -> long (64) */
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 100);
  float f = n;
  assert((long)f == (long)n);

  /* negative rounds toward zero */
  long double neg = -3.75L;
  assert((int)neg == -3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_562
// Needs: --fixedbv
// RUN: --fixedbv
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
