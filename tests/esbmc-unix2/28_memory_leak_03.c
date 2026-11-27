#include <stdlib.h>
#include <pthread.h>

int *shared;

void *t1(void *arg)
{
  return NULL;
}

int main()
{
  shared = (int *)malloc(sizeof(int));
  pthread_t id1;
  pthread_create(&id1, NULL, t1, NULL);
  pthread_join(id1, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/28_memory_leak_03
// Needs: --memory-leak-check --show-stacktrace
// RUN: --memory-leak-check --show-stacktrace --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
