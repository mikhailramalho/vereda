// #9867: an _Atomic pointer does not make its pointee atomic, so *p races.
#include <pthread.h>
int v;
int *_Atomic p = &v;

void *thr(void *arg)
{
  *p = 1;
  return 0;
}

int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, thr, 0);
  *p = 2;
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9867_atomic_ptr_pointee_fail
// Needs: --context-bound 2 --data-races-check-only
// RUN: --context-bound 2 --data-races-check-only --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: W/W data race
// Vereda: verdict none
