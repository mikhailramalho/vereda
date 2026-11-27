/* The other thread must run while main holds the lock, before x = 1 (#9680). */
#include <assert.h>
#include <pthread.h>
pthread_rwlock_t l;
int x;
void *t(void *a)
{
  if (pthread_rwlock_trywrlock(&l) != 0 && x == 0)
    assert(0);
  return 0;
}
int main(void)
{
  pthread_rwlock_init(&l, 0);
  pthread_t th;
  pthread_create(&th, 0, t, 0);
  if (pthread_rwlock_trywrlock(&l) != 0) return 0;
  x = 1;
  pthread_rwlock_unlock(&l);
  pthread_join(th, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9680_trywrlock_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
