/* #7900 §7 question 2: x > 0 is inductive on its own, y != ... is not. */
#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int x = __VERIFIER_nondet_int(), y = 0;
  __VERIFIER_assume(x > 0);
  while (__VERIFIER_nondet_int())
  {
    assert(x > 0);
    assert(y != 3);
    y = 1 - y;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/multi_property_inductive_sibling_unknown
// Needs: --k-induction
// RUN: --k-induction --multi-property --max-k-step 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
