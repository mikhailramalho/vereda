/* Issue #8658: INT_MIN + INT_MIN wraps to 0 and was not reported. */
#include <assert.h>
#include <limits.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int(), r;
  long long exact = (long long)a + b;
  assert(
    __builtin_sadd_overflow(a, b, &r) == (exact < INT_MIN || exact > INT_MAX));
  assert(__builtin_sadd_overflow(INT_MIN, INT_MIN, &r));
  assert(!__builtin_sadd_overflow(0, 0, &r));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8658
// RUN: --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
