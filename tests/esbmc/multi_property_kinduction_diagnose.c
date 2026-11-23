/* The same program without --multi-fail-fast: the base case at k = 2 solves
   every claim, so the diagnostic inductive step at max-k proves the other
   two. */
#include <assert.h>
_Bool __VERIFIER_nondet_bool();
unsigned __VERIFIER_nondet_uint();
int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n < 10);
  unsigned i = 0, j = 3;
  while (__VERIFIER_nondet_bool())
  {
    i++;
    assert(i != 2);
    assert(n < 10);
    assert(j == 3);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_diagnose
// Needs: --k-induction
// RUN: --k-induction --multi-property --max-k-step 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*This report is partial)[\s\S]*\Z
// Vereda: verdict none
