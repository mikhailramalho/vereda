/* #10990: a thread halted by an unwinding bound inside an atomic section
 * never leaves it, so main never sees x == 1. */
#include <pthread.h>
#include <assert.h>
int x;
int __VERIFIER_nondet_int();
void *t(void *arg)
{
  __VERIFIER_atomic_begin();
  x = 1;
  while (__VERIFIER_nondet_int())
    ;
  x = 0;
  __VERIFIER_atomic_end();
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_loop_bound_atomic
// Needs: --partial-order
// RUN: --partial-order --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
