

#include <assert.h>

int main()
{
  int i = 0;
  int sum = 0;

  /*@ loop invariant i >= 0 && i <= 500000000 && sum == i * 10;
     @*/

  __ESBMC_loop_invariant(i >= 0 && i <= 500000000 && sum == i * 10);
  for (i = 0; i < 500000000; i++)
  {
    sum += 10;
  }

  assert(sum == 5000000000);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/11-sum_overflow
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
