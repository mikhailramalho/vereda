/* #10990: a read at a symbolic index sees the initial value or t1's write. */
#include <pthread.h>
#include <assert.h>
int a[2][2];
unsigned __VERIFIER_nondet_uint();
void *t1(void *arg)
{
  a[1][1] = 5;
  return 0;
}
void *t2(void *arg)
{
  unsigned i = __VERIFIER_nondet_uint() % 2, j = __VERIFIER_nondet_uint() % 2;
  int r = a[i][j];
  assert(r == 0 || r == 5);
  return 0;
}
int main()
{
  pthread_t x, y;
  pthread_create(&x, 0, t1, 0);
  pthread_create(&y, 0, t2, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_array_index
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
