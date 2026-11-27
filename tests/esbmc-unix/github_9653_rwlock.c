/* x == 1 is never visible outside main's critical section (#9653). */
#include <assert.h>
#include <pthread.h>
void reach_error(void) { assert(0); }
pthread_rwlock_t s;
int x;
void *t(void *a)
{
  pthread_rwlock_wrlock(&s);
  if (x == 1)
    reach_error();
  pthread_rwlock_unlock(&s);
  return 0;
}
int main(void)
{
  pthread_rwlock_init(&s, 0);
  pthread_t th;
  pthread_create(&th, 0, t, 0);
  pthread_rwlock_wrlock(&s);
  x = 1;
  x = 2;
  pthread_rwlock_unlock(&s);
  pthread_join(th, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9653_rwlock
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
