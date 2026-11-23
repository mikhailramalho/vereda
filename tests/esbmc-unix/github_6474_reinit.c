#include <pthread.h>

static pthread_mutex_t m;

static void *w(void *p)
{
  (void)p;
  pthread_mutex_lock(&m);
  pthread_mutex_unlock(&m);
  return 0;
}

int main(void)
{
  pthread_t a;
  pthread_mutex_init(&m, 0);
  pthread_mutex_lock(&m);
  pthread_create(&a, 0, w, 0);
  pthread_mutex_init(&m, 0);
  pthread_join(a, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6474_reinit
// Needs: --deadlock-check --context-bound 3
// RUN: --deadlock-check --context-bound 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
