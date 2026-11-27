#include <pthread.h>

int esbmc;
pthread_mutex_t mutex;

void *thread1(void *arg)
{
  pthread_mutex_lock(&mutex); 
  esbmc = 1;
//  pthread_mutex_unlock(&mutex);
}

void *thread2(void *arg)
{
  pthread_mutex_lock(&mutex); 
  esbmc = 1;
  pthread_mutex_unlock(&mutex);
}

int main(void)
{
  pthread_t id1, id2;

  pthread_mutex_init(&mutex, NULL);
  pthread_create(&id1, NULL, thread1, NULL);
  pthread_create(&id2, NULL, thread2, NULL);

  pthread_join(id1,NULL);
  pthread_join(id2,NULL);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread37
// Needs: --deadlock-check --context-bound 3
// RUN: --deadlock-check --context-bound 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
