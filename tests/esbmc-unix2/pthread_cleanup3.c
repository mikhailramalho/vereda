#include <pthread.h>
#include <assert.h>

int flag = 0;

void cleanup(void *arg)
{
  flag = 123;
}

void *thread_func(void *arg)
{
  pthread_cleanup_push(cleanup, NULL);
  pthread_cleanup_pop(1);
}

int main()
{
  pthread_t t;
  pthread_create(&t, NULL, thread_func, NULL);
  pthread_join(t, NULL);
  assert(flag == 123); 
}
// Imported from ESBMC regression/esbmc-unix2/pthread_cleanup3
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
