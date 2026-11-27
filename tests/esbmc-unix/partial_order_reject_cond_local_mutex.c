#include <pthread.h>
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int main()
{
  pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_wait(&c, &m);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_cond_local_mutex
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a mutex that is not a global yet, at file .*partial_order_reject_cond_local_mutex\.c line +6
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
