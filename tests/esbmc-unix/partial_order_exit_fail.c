/* #10990: a thread that may call pthread_exit may also run on past it. */
#include <pthread.h>
#include <assert.h>
_Bool __VERIFIER_nondet_bool(void);
int x = 0;
void *t(void *a)
{
  x = 1;
  if (__VERIFIER_nondet_bool())
    pthread_exit(0);
  x = 2;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  assert(x == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_exit_fail
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
