#include <pthread.h>
int x;
void *t(void *arg)
{
  pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&m);
  x = 1;
  pthread_mutex_unlock(&m);
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_local_mutex
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a mutex that is not a global yet, at file .*partial_order_reject_local_mutex\.c line +6
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
