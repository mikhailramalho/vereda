/* #10990: the loop may create no thread at all. */
#include <pthread.h>
#include <assert.h>
int __VERIFIER_nondet_int();
int x;
void *t(void *arg)
{
  x = 1;
  return 0;
}
int main()
{
  pthread_t id[2];
  int n = __VERIFIER_nondet_int();
  if (n < 0 || n > 2)
    n = 2;
  for (int i = 0; i < n; ++i)
    pthread_create(&id[i], 0, t, 0);
  for (int i = 0; i < n; ++i)
    pthread_join(id[i], 0);
  assert(x == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_loop_spawn_guarded_fail
// Needs: --partial-order
// RUN: --partial-order --unwind 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
