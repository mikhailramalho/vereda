/* The lock taken right after main releases it must see x == 1 (#9653). */
#include <assert.h>
#include <pthread.h>
void reach_error(void) { assert(0); }
pthread_mutex_t s;
int x;
void *t(void *a)
{
  pthread_mutex_lock(&s);
  if (x == 1)
    reach_error();
  pthread_mutex_unlock(&s);
  return 0;
}
int main(void)
{
  pthread_mutex_init(&s, 0);
  pthread_t th;
  pthread_create(&th, 0, t, 0);
  pthread_mutex_lock(&s);
  x = 1;
  pthread_mutex_unlock(&s);
  x = 2;
  pthread_join(th, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9653_mutex_lock_order_check_fail
// Needs: --lock-order-check
// RUN: --lock-order-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
