#include <pthread.h>

void *malloc(unsigned size);
pthread_mutex_t mutex;

#if 1
void *thread1(void *arg)
{
  pthread_mutex_lock(&mutex); 
  pthread_mutex_unlock(&mutex);
}
#endif

#if 1
void *thread2(void *arg)
{
//  pthread_mutex_lock(&mutex); 
  pthread_mutex_unlock(&mutex);
}
#endif

int main(void)
{
  pthread_t id1, id2;

//  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(&mutex, NULL);
//  assert(0);
  pthread_create(&id1, NULL, thread1, NULL);
  pthread_create(&id2, NULL, thread2, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/01_malloc_15
// Needs: --lock-order-check
// RUN: --lock-order-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
