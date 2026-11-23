/* Without --multi-property, --falsification stops at the first violation,
   so the k = 4 violation is never reached (esbmc/esbmc#7900). */
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
// Imported from ESBMC regression/esbmc/falsification_stops_at_first_violation
// Needs: --falsification
// RUN: --falsification --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Bug found \(k = 1\)$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*Checking base case, k = 2)[\s\S]*\Z
// Vereda: verdict none
