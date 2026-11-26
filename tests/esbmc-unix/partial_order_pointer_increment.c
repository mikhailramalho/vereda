/* #10990: increments through pointers, each in an atomic section. */
#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  int *p = &x;
  __VERIFIER_atomic_begin();
  *p = *p + 1;
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
// Imported from ESBMC regression/esbmc-unix/partial_order_pointer_increment
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
