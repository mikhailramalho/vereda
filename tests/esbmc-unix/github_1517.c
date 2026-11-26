#include <pthread.h>
#include <semaphore.h>

int count = 0;
sem_t sem; // declare the semaphore

void *increment(void *arg)
{
  sem_wait(&sem); // decrement the semaphore
  count++;
  sem_post(&sem); // increment the semaphor
  return NULL;
}

int main()
{
  pthread_t t1, t2;
  sem_init(&sem, 0, 1); // initialize the semaphore
  pthread_create(&t1, NULL, increment, NULL);
  pthread_create(&t2, NULL, increment, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  sem_destroy(&sem); // destroy the semaphore
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1517
// Needs: --data-race-check --context-bound 2
// RUN: --unwind 2 --no-unwinding-assertions --data-race-check --context-bound 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
