/* #10990: main ending with pthread_exit does not hold back the claims of the
   thread that runs on, which may run before main's write. */
#include <pthread.h>
#include <assert.h>
int x = 0;
void *t(void *a)
{
  assert(x == 1);
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  x = 1;
  pthread_exit(0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_exit_main_fail
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
