#include <assert.h>
#include <pthread.h>

int g;
pthread_once_t once = PTHREAD_ONCE_INIT;

void init(void)
{
  g = 1;
}

void *worker(void *arg)
{
  pthread_once(&once, init);
  assert(g == 1);
  return NULL;
}

int main(void)
{
  pthread_t t;
  pthread_create(&t, NULL, worker, NULL);
  pthread_once(&once, init);
  assert(g == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/pthread_once_norace
// Needs: --data-races-check-only
// RUN: --data-races-check-only --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
