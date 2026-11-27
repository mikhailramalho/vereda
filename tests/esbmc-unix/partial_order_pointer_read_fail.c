/* #10990: before the join, a read through p may miss t's write. */
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
  int *p = &x;
  pthread_create(&id, 0, t, 0);
  assert(*p == 1);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_pointer_read_fail
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
