/* A base case discharges a claim only within k, so --falsification, which
   never proves anything, must not report x != 3 as passed when it stops at
   k = 3: it is violated at k = 4. */
#include <assert.h>

int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned x = 0;
  assert(n != 7);
  for (unsigned i = 0; i < n; ++i)
    ++x;
  assert(x != 3);
}
// Imported from ESBMC regression/esbmc/multi_property_bounded_base_case_row
// Needs: --falsification
// RUN: --falsification --multi-property --max-k-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Properties: 2 checked, \d+ passed, 1 failed, \d+ not checked, 1 unknown$
// Vereda: verdict none
