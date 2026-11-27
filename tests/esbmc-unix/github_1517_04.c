#include <pthread.h>
#include <semaphore.h>

int count = 0;
sem_t sem; // declare the semaphore

void *increment(void *arg)
{
  sem_wait(&sem); // decrement the semaphore
  count++;
  sem_wait(&sem); // decrement the semaphore twice
  return NULL;
}

int main()
{
  pthread_t t1, t2;
  sem_init(&sem, 0, 1);  //initialise check
  pthread_create(&t1, NULL, increment, NULL);
  pthread_create(&t2, NULL, increment, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  sem_destroy(&sem); // destroy the semaphore
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1517_04
// Needs: --deadlock-check --context-bound 2
// RUN: --deadlock-check --context-bound 2 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
