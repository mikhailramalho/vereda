/* Both assertions hold and the forward condition closes at k = 5, so the
   coverage report printed on that exit must show both assertion instances
   reached. */
#include <assert.h>
int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned x = 0;
  __VERIFIER_assume(n < 5);
  for (unsigned i = 0; i < n; ++i)
    ++x;
  assert(x == n);
  assert(x < 5);
}
// Imported from ESBMC regression/esbmc/incremental_bmc_assertion_coverage_forward_condition
// Needs: --assertion-coverage
// RUN: --incremental-bmc --assertion-coverage --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Solution found by the forward condition; all states are reachable \(k = 5\)$
// EXPECT: ^Reached Assertion Instances: 2$
// EXPECT: ^Assertion Instances Coverage: 100%$
// Vereda: verdict none
