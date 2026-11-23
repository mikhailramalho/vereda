/* github #8706 */
#include <assert.h>
#include <limits.h>
unsigned __VERIFIER_nondet_uint();
int __VERIFIER_nondet_int();
int main(void)
{
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  __VERIFIER_assume(a == 0x80000000u && b == 0xffffffffu);
  unsigned q = a / b;
  assert(q == 1);
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == INT_MIN && y == -1);
  int s = x / y;
  return s;
}
// Imported from ESBMC regression/esbmc/github_8706_fail
// RUN: --overflow-check --unsigned-overflow-check --solver bitwuzla --multi-property
// EXPECT: ^  PASSED .*line +10  arithmetic overflow on div$
// EXPECT: ^  FAILED .*line +11  assertion q == 1$
// EXPECT: ^  FAILED .*line +14  arithmetic overflow on div$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  PASSED .*line +10  arithmetic overflow on div$
// MODE: FUTURE
