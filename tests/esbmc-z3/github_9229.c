#include <limits.h>
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  if (a <= UINT_MAX - b)
    return a + b;
  return 0;
}
// Imported from ESBMC regression/z3/github_9229
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^  PASSED .*line   7  arithmetic overflow on add$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^  PASSED .*line   7  arithmetic overflow on add$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
