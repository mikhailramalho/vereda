/* #10990: the deadlock claim takes main's end for the process's end, which
   pthread_exit breaks. */
#include <pthread.h>
void *t(void *arg)
{
  pthread_exit(0);
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_deadlock_exit
// Needs: --partial-order --deadlock-check
// RUN: --partial-order --deadlock-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support checking deadlocks with a call to pthread_exit yet, at file .*partial_order_reject_deadlock_exit\.c line +6 column 3 function
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
