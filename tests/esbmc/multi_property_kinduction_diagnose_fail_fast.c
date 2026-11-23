/* The base case at k = 2 finds the first claim violated in the second
   iteration, and --multi-fail-fast skips the other claims' instances there.
   The diagnostic inductive step at max-k proves both inductive -- the third
   by the simplifier alone -- but that round did not back either proof. */
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
// Imported from ESBMC regression/esbmc/multi_property_kinduction_diagnose_fail_fast
// Needs: --k-induction --multi-fail-fast 1
// RUN: --k-induction --multi-property --multi-fail-fast 1 --max-k-step 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^This report is partial: the run stopped before every property reached a verdict
// Vereda: verdict none
