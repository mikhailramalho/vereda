/* Issue #8097: under --32 long double was a 96-bit (15, 81) float, a format
 * Bitwuzla only builds when compiled with --fpexp, so it aborted. long double
 * is now binary128 there, as under --64. x is nondet so the long double sort
 * reaches the solver. */
#include <assert.h>

long double __VERIFIER_nondet_ld(void);

int main()
{
  long double x = __VERIFIER_nondet_ld();
  __VERIFIER_assume(x == 1.0L / 3);
  /* 1/3 has no exact double, so the narrowing loses precision. */
  assert((double)x != x);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/github_8097
// RUN: --solver bitwuzla -m32 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
