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
// Imported from ESBMC regression/esbmc/github_6382-default
// Needs: --context-bound 2 --no-standard-checks
// RUN: --multi-property --unwind 11 --context-bound 2 --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^ +PASSED .*Sem is not initialized$
// EXPECT: \A(?![\s\S]*library assertion)[\s\S]*\Z
// Vereda: verdict none
