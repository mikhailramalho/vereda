/* #10990: main's spawning loop is cut by its unwinding bound, which must stop
 * only main: the spawned threads still run and can see each other's s++. */
#include <pthread.h>
#include <assert.h>
int s;
void *t(void *arg)
{
  s++;
  assert(s == 1);
  s--;
  return 0;
}
int main()
{
  pthread_t id;
  while (1)
    pthread_create(&id, 0, t, 0);
}
// Imported from ESBMC regression/esbmc-unix/partial_order_loop_bound_fail
// Needs: --partial-order
// RUN: --partial-order --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
