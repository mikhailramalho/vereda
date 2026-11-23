/* Neither assert(*p == 1) nor the checks raised inside it can fail. */
#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int x = 1, y = 1;
  int *p = __VERIFIER_nondet_int() ? &x : &y;
  assert(*p == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_check_in_assert
// Needs: --k-induction --multi-fail-fast 1
// RUN: --k-induction --multi-property --multi-fail-fast 1 --max-k-step 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
