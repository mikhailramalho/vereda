/* #10990: a write on one branch still races with the other thread's read. */
#include <pthread.h>
int g;
int __VERIFIER_nondet_int();
void *reader(void *arg)
{
  int l = g;
  return 0;
}
void *writer(void *arg)
{
  if (__VERIFIER_nondet_int())
    g = 1;
  return 0;
}
int main()
{
  pthread_t a, b;
  pthread_create(&a, 0, reader, 0);
  pthread_create(&b, 0, writer, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_race_guarded_fail
// Needs: --partial-order --ub-data-race-check
// RUN: --partial-order --ub-data-race-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
