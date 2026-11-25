/* The second claim is violated only at k = 4, so --falsification must keep
   escalating past the first violation to report it (esbmc/esbmc#7900). */
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
// Imported from ESBMC regression/esbmc/multi_property_falsification
// Needs: --falsification
// RUN: --falsification --multi-property --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Properties: 2 checked, \d+ passed, 2 failed, \d+ not checked$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
