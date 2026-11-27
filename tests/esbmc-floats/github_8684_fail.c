/* esbmc/esbmc#8684: the selected -0.0 is negative, so both asserts fail. */
#include <assert.h>
#include <math.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int c = __VERIFIER_nondet_int();
  double a = c ? -0.0 : 1.0;
  double b = c ? 1.0 : -0.0;
  assert(!c || !signbit(a));
  assert(c || !signbit(b));
  return 0;
}
// Imported from ESBMC regression/floats/github_8684_fail
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+10\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+11\s
// EXPECT: ^VERIFICATION FAILED$
