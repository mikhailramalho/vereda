/* github #8706: Bitwuzla flagged unsigned / and % as signed division
 * overflow when the operands' bit patterns are INT_MIN and -1. */
#include <assert.h>
unsigned __VERIFIER_nondet_uint();
int main(void)
{
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  __VERIFIER_assume(a == 0x80000000u && b == 0xffffffffu);
  unsigned q = a / b;
  unsigned r = a % b;
  assert(q == 0 && r == 0x80000000u);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8706
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --solver bitwuzla
// EXPECT: ^VERIFICATION SUCCESSFUL$
