#include <assert.h>
int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned x = 0;
  __VERIFIER_assume(n < 5);
  for (unsigned i = 0; i < n; ++i)
    ++x;
  __VERIFIER_assume(x > 100);
  assert(x == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_vacuous
// Needs: --k-induction --check-vacuity
// RUN: --k-induction --multi-property --check-vacuity --max-k-step 8
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)[\s\S]*\Z
// Vereda: verdict none
