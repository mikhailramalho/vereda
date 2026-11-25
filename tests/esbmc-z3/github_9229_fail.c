#include <limits.h>
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  if (a >= UINT_MAX - b)
    return a + b;
  return 0;
}
// Imported from ESBMC regression/z3/github_9229_fail
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^  arithmetic overflow on add$
// EXPECT: ^VERIFICATION FAILED$
