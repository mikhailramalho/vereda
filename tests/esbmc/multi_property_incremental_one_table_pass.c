/* Safe twin: the forward condition closes at k = 5 and proves both claims.
   Its own unwinding assertion stays out of the table. */
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
// Imported from ESBMC regression/esbmc/multi_property_incremental_one_table_pass
// RUN: --incremental-bmc --multi-property --max-k-step 10
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^No bug has been found in the base case$
// Vereda: unmet EXPECT: ^No bug has been found in the base case$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
