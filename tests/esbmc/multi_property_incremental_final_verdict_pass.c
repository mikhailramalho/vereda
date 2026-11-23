/* The safe twin of multi_property_incremental_final_verdict: the forward
   condition closes with no violation recorded, so the run must still end
   VERIFICATION SUCCESSFUL (esbmc/esbmc#7900). */
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
// Imported from ESBMC regression/esbmc/multi_property_incremental_final_verdict_pass
// RUN: --incremental-bmc --multi-property --max-k-step 10
// EXPECT: ^Solution found by the forward condition; all states are reachable \(k = 5\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED)[\s\S]*\Z
// Vereda: unmet EXPECT: ^Solution found by the forward condition; all states are reachable \(k = 5\)$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
