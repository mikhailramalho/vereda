/* #10990: main releases the mutex before it waits for the thread. */
#include <pthread.h>
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
void *t(void *arg)
{
  pthread_mutex_lock(&m);
  pthread_mutex_unlock(&m);
  return 0;
}
int main()
{
  pthread_t id;
  pthread_mutex_lock(&m);
  pthread_create(&id, 0, t, 0);
  pthread_mutex_unlock(&m);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_deadlock_join
// Needs: --partial-order --deadlock-check
// RUN: --partial-order --deadlock-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
