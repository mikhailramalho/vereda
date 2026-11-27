#include <pthread.h>
#include <assert.h>

pthread_mutex_t  x, y;

void *thread1(void *arg)
{
  pthread_mutex_lock(&x);
  pthread_mutex_unlock(&x);
  pthread_mutex_lock(&x);

  pthread_mutex_lock(&y);
  pthread_mutex_unlock(&y);
  pthread_mutex_lock(&y);
  pthread_mutex_unlock(&y);
}

void *thread2(void *arg)
{
  pthread_mutex_lock(&x);
  pthread_mutex_unlock(&x);
  pthread_mutex_lock(&x); 

  pthread_mutex_lock(&y);
  pthread_mutex_unlock(&y);
  pthread_mutex_lock(&y);
  pthread_mutex_unlock(&y);
}

int main()
{
  pthread_mutex_init(&x, NULL);
  pthread_mutex_init(&y, NULL);

  pthread_t t1, t2;

  pthread_create(&t1, 0, thread1, 0);
  pthread_create(&t2, 0, thread2, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/02_phase_03
// Needs: --deadlock-check --context-bound 3
// RUN: --deadlock-check --context-bound 3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
