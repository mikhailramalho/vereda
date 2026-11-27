/* A model that violates only the claim past the havoc (j == 0, a weak
   invariant) must not hide the real bug on the other branch (#10641). */
#include <assert.h>
int __VERIFIER_nondet_int();
int main()
{
  int c = __VERIFIER_nondet_int(), x = __VERIFIER_nondet_int(), i = 0, j = 0;
  if (!c)
    assert(x != 3);
  else
  {
    __ESBMC_loop_invariant(i <= 10);
    while (i < 10)
    {
      i++;
      j = 0;
    }
    assert(j == 0);
  }
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_10641_weak_model_hides_bug_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
