// Issue #8220: t2 runs unprotected after main unlocks, a race that needs
// four context switches.
#include <pthread.h>

int global = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t id1, id2;

void *t1(void *arg)
{
  pthread_mutex_lock(&mutex);
  global++;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void *t2(void *arg)
{
  global++;
  return NULL;
}

int main(void)
{
  pthread_create(&id1, NULL, t1, NULL);
  pthread_mutex_lock(&mutex);
  pthread_create(&id2, NULL, t2, NULL);
  pthread_join(id2, NULL);
  pthread_create(&id2, NULL, t2, NULL);
  pthread_mutex_unlock(&mutex);
  pthread_join(id2, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8220_reused_thread_race
// Needs: --sv-comp --data-races-check-only --timeout 60s
// RUN: --sv-comp --z3 --data-races-check-only --no-assertions --ub-pointer-check=false --no-bounds-check --timeout 60s
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
