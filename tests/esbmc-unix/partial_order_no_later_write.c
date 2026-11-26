/* #10990: a read never observes a write that comes after it. */
#include <pthread.h>
#include <assert.h>
int x;
void *t1(void *arg)
{
  int r = x;
  x = 1;
  assert(r == 0);
  return 0;
}
int main()
{
  pthread_t a;
  pthread_create(&a, 0, t1, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_no_later_write
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
