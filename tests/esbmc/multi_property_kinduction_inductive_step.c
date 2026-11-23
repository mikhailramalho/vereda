#include <assert.h>
int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned i = 0;
  while (i < n)
  {
    ++i;
    assert(i <= n);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_inductive_step
// Needs: --k-induction
// RUN: --k-induction --multi-property --max-k-step 8
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^No bug has been found in the base case$
// Vereda: verdict none
