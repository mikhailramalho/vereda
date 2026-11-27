/* #10990: before the join, main may see any of the thread's writes. */
#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  x = 1;
  x = 2;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  int r = x;
  pthread_join(id, 0);
  assert(r == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_join_fail
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
