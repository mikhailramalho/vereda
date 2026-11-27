/* No-race control for github_2928_escape_rc: identical shape -- main's stack
   local escapes to the spawned thread -- except both sides take the same
   mutex, so the accesses are ordered and no race exists. Pairs with that test
   so recognising an escaped stack local as shared cannot degrade into
   reporting a race on every such access. */
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *t_fun(void *arg)
{
  int *p = (int *)arg;
  pthread_mutex_lock(&mutex);
  (*p)++;
  pthread_mutex_unlock(&mutex);
  return 0;
}

int main(void)
{
  pthread_t id;
  int i = 0;
  pthread_create(&id, 0, t_fun, (void *)&i);
  pthread_mutex_lock(&mutex);
  i++;
  pthread_mutex_unlock(&mutex);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2928_escape_norace
// Needs: --sv-comp --force-realloc-success --data-races-check-only
// RUN: -m32 --sv-comp --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --ub-pointer-check=false --no-bounds-check --data-races-check-only --no-assertions --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
