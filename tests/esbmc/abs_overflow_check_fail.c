#include <limits.h>
#include <stdlib.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x <= -2147483647);
  return abs(x);
}
// Imported from ESBMC regression/esbmc/abs_overflow_check_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: arithmetic overflow on abs
