/* #10990: t1's write may come before t2's read. */
#include <pthread.h>
#include <assert.h>
int x;
void *t1(void *arg)
{
  x = 1;
  return 0;
}
void *t2(void *arg)
{
  int r = x;
  assert(r == 0);
  return 0;
}
int main()
{
  pthread_t a, b;
  pthread_create(&a, 0, t1, 0);
  pthread_create(&b, 0, t2, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_rf_value_fail
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
