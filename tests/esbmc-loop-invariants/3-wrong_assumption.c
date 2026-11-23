
#include <assert.h>

int main()
{
  int i = 0;
  int sum = 0;

  /*@ loop invariant i >= 0 && i < 5000000 && sum == i * 10; overflow should fail 
        @*/

  //first check the base case

  __ESBMC_loop_invariant(i >= 0 && i < 5000000 && sum == i * 10);
  while (i < 5000000) //replace the condition to i <5000000
  {
    sum += 10;
    i++;
  }
  assert(sum == 50000000);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/3-wrong_assumption
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
