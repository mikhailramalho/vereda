/* #10990: pthread_exit ends the thread, so its later write never happens. */
#include <pthread.h>
#include <assert.h>
int x = 0;
void *t(void *a)
{
  x = 1;
  pthread_exit(0);
  x = 2;
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
// Imported from ESBMC regression/esbmc-unix/partial_order_exit
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
