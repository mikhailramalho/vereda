/* #10990: the operational model's thread counters are not events. */
#include <pthread.h>
#include <assert.h>
extern unsigned short int __ESBMC_num_threads_running;
void *t(void *a)
{
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  assert(__ESBMC_num_threads_running <= 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_scheduler_state
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support the scheduler state __ESBMC_num_threads_running yet, at file .*partial_order_reject_scheduler_state\.c line +13 column 3 function main$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
