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
  pthread_mutex_lock(&mutex);
  global++;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(void)
{
  pthread_create(&id1, NULL, t1, NULL);
  pthread_mutex_lock(&mutex);
  pthread_create(&id2, NULL, t2, NULL);
  pthread_mutex_unlock(&mutex);
  pthread_join(id2, NULL);
  pthread_join(id1, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/github_8220_reused_thread_id
// Needs: --context-bound 3 --data-races-check-only --no-assertions
// RUN: --context-bound 3 --data-races-check-only --no-assertions --no-pointer-check --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
