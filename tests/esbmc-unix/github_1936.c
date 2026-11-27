#include <pthread.h>
#include <assert.h>

int x;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void* taskA(void* arg)
{
  pthread_mutex_lock(&m);
  x++;
  pthread_mutex_unlock(&m);
  return NULL;
}

void* taskB(void* arg)
{
  pthread_mutex_lock(&m);
  if(!x)
    assert(1);
  pthread_mutex_unlock(&m);
  return NULL;
}

int main() {
  pthread_t idA, idB;

  pthread_create(&idA, NULL, taskA, NULL);
  pthread_create(&idB, NULL, taskB, NULL);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1936
// Needs: --ub-data-race-check --context-bound 2
// RUN: --ub-data-race-check --context-bound 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
