#include <math.h>

int main()
{
  double x = __VERIFIER_nondet_float();

  __VERIFIER_assume(isgreaterequal(x, 0.0));

  __VERIFIER_assert(x == x);

  return 0;
}
// Imported from ESBMC regression/esbmc/github_1226-2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
