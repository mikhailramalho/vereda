/* The NULL-pointer check raised inside assert(*p == 2) fails at k = 1 and
   --multi-fail-fast skips the rest. The assertion is its own claim: the
   check's violation must not drop it, or no base case ever solves it. */
#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int x = 1;
  int *p = __VERIFIER_nondet_int() ? &x : 0;
  assert(*p == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_check_in_assert_fail
// Needs: --k-induction --multi-fail-fast 1
// RUN: --k-induction --multi-property --multi-fail-fast 1 --max-k-step 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s+FAILED\s+\[main\.null-pointer-dereference\.1\]\s+line\s+12\s+dereference failure: NULL pointer$
// Vereda: verdict none
