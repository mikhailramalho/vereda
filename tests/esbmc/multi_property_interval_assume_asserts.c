/* esbmc/esbmc#7900: --interval-analysis-assume-asserts narrowed the state to
   x > 10 after the first assertion, so the second was folded away although
   --multi-property checks it on the paths where the first one fails. */
#include <assert.h>
int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x > 10);
  assert(x > 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_interval_assume_asserts
// Needs: --interval-analysis --interval-analysis-assume-asserts
// RUN: --interval-analysis --interval-analysis-assume-asserts --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
