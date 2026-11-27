/* #10296's counter with atomic sections for the mutex, and three threads. */
#include <pthread.h>
#include <assert.h>
int x = 0;
void *t(void *a)
{
  __VERIFIER_atomic_begin();
  x = x + 1;
  __VERIFIER_atomic_end();
  return 0;
}
int main()
{
  pthread_t a, b, c;
  pthread_create(&a, 0, t, 0);
  pthread_create(&b, 0, t, 0);
  pthread_create(&c, 0, t, 0);
  pthread_join(a, 0);
  pthread_join(b, 0);
  pthread_join(c, 0);
  assert(x == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_github_10296_counter
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
