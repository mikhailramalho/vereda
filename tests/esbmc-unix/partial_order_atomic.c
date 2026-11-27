/* #10990: increments inside atomic sections are not lost. */
#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  __VERIFIER_atomic_begin();
  x = x + 1;
  __VERIFIER_atomic_end();
  return 0;
}
int main()
{
  pthread_t a, b;
  pthread_create(&a, 0, t, 0);
  pthread_create(&b, 0, t, 0);
  pthread_join(a, 0);
  pthread_join(b, 0);
  assert(x == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_atomic
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
