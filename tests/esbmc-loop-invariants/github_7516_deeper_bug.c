/* #7516: a base case that only refutes the loop-invariant abstraction must
   not stop the search for a real bug at a larger k. */
#include <assert.h>
int __VERIFIER_nondet_int();
int main()
{
  int n = __VERIFIER_nondet_int();
  int j = 0;
  while (j < n)
  {
    j++;
    assert(j < 5);
  }
  int i = 0, s = 0;
  __ESBMC_loop_invariant(i >= 0);
  while (i < 3)
  {
    s++;
    i++;
  }
  assert(s == 3);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7516_deeper_bug
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --k-induction-parallel
// RUN: --loop-invariant-check --k-induction-parallel --max-k-step 8
// MODE: FUTURE
// EXPECT: ^Bug found by the base case \(k = [0-9]+\)$
// EXPECT: ^VERIFICATION FAILED$
