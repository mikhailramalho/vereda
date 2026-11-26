/* #10990: an array is one object, so its elements cannot be checked apart. */
#include <pthread.h>
int a[2];
void *t(void *arg)
{
  a[1] = 1;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  a[0] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_race_array
// Needs: --partial-order --data-race-check
// RUN: --partial-order --data-race-check
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support checking data races on a global array yet, at file .*partial_order_reject_race_array\.c line +13 column 3 function main$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
