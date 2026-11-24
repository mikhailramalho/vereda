#include <pthread.h>
#include <assert.h>

int esbmc;
pthread_mutex_t mutex;

void *thread1(void *arg)
{
  pthread_mutex_lock(&mutex); 
  esbmc = 1;
  pthread_mutex_unlock(&mutex);
}

int main(void)
{
  pthread_t id1;

  pthread_mutex_init(&mutex, NULL);
  pthread_create(&id1, NULL, thread1, NULL);

  pthread_mutex_lock(&mutex);
  esbmc=2;
  assert(esbmc==2);
  pthread_mutex_unlock(&mutex);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread36
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
