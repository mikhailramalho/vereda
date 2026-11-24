/*
 * Weiqi Wang
 * Case for there is a non-assigned variable away from the loop invariant
 * Should fail line 25 and overflow in line 21
*/

#include <assert.h>

int main()
{
  int i = 0;
  int sum = 0;
  int secret = 42;

  /*@ loop invariant i >= 0 && i <= 5000000 && sum == i * 10;
     @*/

  __ESBMC_loop_invariant(i >= 0 && i <= 5000000 && sum == i * 10);
  while (i < 5000000)
  {
    sum += 10;
    i++;
    secret = secret + 1;
  }

  assert(sum == 50000000);
  assert(secret == 5000042); // 42+5000000
  return 0;
}
// Imported from ESBMC regression/loop-invariants/1-secret_in_loop
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^WARNING: every violated claim lies downstream of a loop invariant havoc
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
