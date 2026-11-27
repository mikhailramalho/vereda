/* #10990: after the join, main reads t's write. */
#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  x = 1;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  int m = x;
  assert(m == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_trace_write
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
