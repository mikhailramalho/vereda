/* esbmc/esbmc#8684: c ? -0.0 : 1.0 must keep the sign of the selected zero. */
#include <assert.h>
#include <math.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int c = __VERIFIER_nondet_int();
  double a = c ? -0.0 : 1.0;
  double b = c ? 1.0 : -0.0;
  assert(!c || signbit(a));
  assert(c || signbit(b));
  assert(signbit(fmin(-0.0, 1.0)));
  return 0;
}
// Imported from ESBMC regression/floats/github_8684
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
