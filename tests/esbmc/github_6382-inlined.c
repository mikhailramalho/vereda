#include <pthread.h>
#include <semaphore.h>

sem_t sem;

int data;
int data_vfy;

void *prod_task(void *arg)
{
  data = data_vfy;
  sem_post(&sem);
  return NULL;
}

void *cons_task(void *arg)
{
  sem_wait(&sem);
  __VERIFIER_assert(data == data_vfy); // assert data equal
  return NULL;
}

int main()
{
  data_vfy = __VERIFIER_nondet_int();
  sem_init(&sem, 0, 0);

  pthread_t prod_thrd;
  pthread_t cons_thrd;

  pthread_create(&prod_thrd, NULL, prod_task, NULL);
  pthread_create(&cons_thrd, NULL, cons_task, NULL);

  pthread_join(prod_thrd, NULL);
  pthread_join(cons_thrd, NULL);

  sem_destroy(&sem);
}
// Imported from ESBMC regression/esbmc/github_6382-inlined
// Needs: --context-bound 2 --no-standard-checks --no-library-assertions
// RUN: --multi-property --unwind 11 --context-bound 2 --no-standard-checks --no-library-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Removed [1-9]\d* library assertion\(s\)$
// EXPECT: \A(?![\s\S]*Sem is not initialized)[\s\S]*\Z
// EXPECT: \A(?![\s\S]*Value has exceeded the maximum)[\s\S]*\Z
// Vereda: verdict none
