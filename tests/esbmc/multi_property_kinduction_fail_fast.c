/* --multi-fail-fast skips the second claim at k = 1, once the first is
   violated. The base case at k = 2 solves it, the first being settled, so
   the forward condition's proof at k = 2 reaches it: what k = 1 skipped
   does not count against a later k. */
#include <assert.h>

int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned m = __VERIFIER_nondet_uint();
  assert(n != 7);
  assert(m + 1 > m || m == 0xffffffffu);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_fail_fast
// Needs: --k-induction --multi-fail-fast 1
// RUN: --k-induction --multi-property --multi-fail-fast 1 --max-k-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*This report is partial)[\s\S]*\Z
// Vereda: verdict none
