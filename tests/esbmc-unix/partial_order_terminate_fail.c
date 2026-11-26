/* #10990: a thread that may call __ESBMC_terminate_thread may also run on
 * past it. Without --partial-order ESBMC reports success: the intrinsic ends
 * the thread on every path, not only under its guard. */
#include <pthread.h>
#include <assert.h>
_Bool __VERIFIER_nondet_bool(void);
void __ESBMC_terminate_thread(void);
int x = 0;
void *t(void *a)
{
  x = 1;
  if (__VERIFIER_nondet_bool())
    __ESBMC_terminate_thread();
  x = 2;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  assert(x != 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_terminate_fail
// Needs: __ESBMC_terminate_thread
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
