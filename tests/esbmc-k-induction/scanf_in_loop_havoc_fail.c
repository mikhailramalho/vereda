// scanf writes through &x, so the inductive step havocs x. Unhavoced, x would
// keep its initial 0 in a step that starts at iteration 20, past the read in
// iteration 10, and the step would prove the false assertion at k = 2.
#include <stdio.h>

int main(void)
{
  int x = 0;
  unsigned i = 0;
  while (1)
  {
    if (i == 10)
      scanf("%d", &x);
    if (i == 20)
      __VERIFIER_assert(x == 0); // scanf may read a non-zero x
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/scanf_in_loop_havoc_fail
// Needs: --k-induction --max-inductive-step 3
// RUN: --k-induction --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
