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
  pthread_join(id1, NULL);
  free(shared);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/28_memory_leak_01
// Needs: --memory-leak-check --show-stacktrace
// RUN: --no-slice --memory-leak-check --show-stacktrace
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
