/* Issue #8658: INT_MIN + INT_MIN wraps to 0 and was not reported. */
#include <limits.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a == INT_MIN);
  int r = a + a;
  return r;
}
// Imported from ESBMC regression/esbmc/github_8658_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --solver z3 --no-incremental-bmc
// EXPECT: ^  FAILED .*arithmetic overflow on add
// EXPECT: ^VERIFICATION FAILED$
