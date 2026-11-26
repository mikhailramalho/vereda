#include <pthread.h>
int g;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
void *t(void *arg)
{
  pthread_mutex_lock(&m);
  g = 1;
  pthread_mutex_unlock(&m);
  return 0;
}
int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_mutex_lock(&m);
  g = 2;
  pthread_mutex_unlock(&m);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9688_race_witness
// Needs: --data-races-check
// RUN: --data-races-check --no-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
