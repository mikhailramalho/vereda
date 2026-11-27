/* #10990: writes of g inside atomic sections do not race. */
#include <pthread.h>
int g;
void *inside(void *arg)
{
  __VERIFIER_atomic_begin();
  g = 1;
  __VERIFIER_atomic_end();
  return 0;
}
void *also_inside(void *arg)
{
  __VERIFIER_atomic_begin();
  g = 2;
  __VERIFIER_atomic_end();
  return 0;
}
int main()
{
  pthread_t a, b;
  pthread_create(&a, 0, inside, 0);
  pthread_create(&b, 0, also_inside, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_race_ww
// Needs: --partial-order --ub-data-race-check
// RUN: --partial-order --ub-data-race-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
