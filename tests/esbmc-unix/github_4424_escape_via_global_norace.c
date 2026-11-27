/* escaped local reached through a GLOBAL pointer instead of the thread arg */
#include <pthread.h>
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER, m2 = PTHREAD_MUTEX_INITIALIZER;
int *g;
void *t_fun(void *arg) {
  pthread_mutex_lock(&m1);
  (*g)++;            /* RACE */
  pthread_mutex_unlock(&m1);
  return 0;
}
int main(void) {
  pthread_t id; int i = 0; g = &i;
  pthread_create(&id, 0, t_fun, 0);
  pthread_mutex_lock(&m1);
  i++;               /* RACE */
  pthread_mutex_unlock(&m1);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_4424_escape_via_global_norace
// Needs: --sv-comp --data-races-check-only
// RUN: -m32 --sv-comp --ub-div-by-zero-check=false --force-malloc-success --ub-pointer-check=false --data-races-check-only --no-assertions --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
