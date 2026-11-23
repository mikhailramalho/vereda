/* #9441: c = c + 1 is an atomic load and a separate atomic store, so the two
 * threads can both read 0 and the final value can be 1. */
#include <assert.h>
#include <pthread.h>

_Atomic char c = 0;

void *t(void *arg)
{
  c = c + 1;
  return 0;
}

int main(void)
{
  pthread_t t1, t2;
  pthread_create(&t1, 0, t, 0);
  pthread_create(&t2, 0, t, 0);
  pthread_join(t1, 0);
  pthread_join(t2, 0);
  assert(c == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9441_atomic_narrow_rmw_fail
// Needs: --no-standard-checks
// RUN: --64 --no-standard-checks --unwind 20
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
