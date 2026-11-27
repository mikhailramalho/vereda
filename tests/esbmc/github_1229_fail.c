#include <math.h>

int main()
{
  double x = __VERIFIER_nondet_double();

  __VERIFIER_assume(isunordered(x, 0.0));

  __VERIFIER_assert(isgreaterequal(x, 0.0)); // unordered compares greater-equal

  return 0;
}
// Imported from ESBMC regression/esbmc/github_1229_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
