#include <pthread.h>
#include <semaphore.h>

sem_t sem;

int data;

void *producer(void *arg)
{
  data = 99;
  sem_post(&sem);
}

void *consumer(void *arg)
{
  sem_wait(&sem);
  __VERIFIER_assert(data == 99); // assert data equal
}

int main()
{
  sem_init(&sem, 0, 0);
  pthread_t prod;
  pthread_t cons;
  pthread_create(&prod, NULL, producer, NULL);
  pthread_create(&cons, NULL, consumer, NULL);
}
// Imported from ESBMC regression/esbmc-unix/sem_init_zero_value
// Needs: --context-bound 2
// RUN: --context-bound 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
