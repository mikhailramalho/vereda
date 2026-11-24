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
  pthread_exit(NULL); // cleanup should run
  pthread_cleanup_pop(1); // never reached
}

int main()
{
  pthread_t t;
  pthread_create(&t, NULL, thread_func, NULL);
  pthread_join(t, NULL);
  assert(flag == 123); // executed at pthread_exit
}
// Imported from ESBMC regression/esbmc-unix2/pthread_cleanup6
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
