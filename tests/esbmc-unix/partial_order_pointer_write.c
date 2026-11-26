/* #10990: a write through a pointer to a global is an event. */
#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  int *p = &x;
  *p = 1;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  assert(x == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_pointer_write
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
