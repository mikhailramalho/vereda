/* #10990: main writes g before joining the thread that writes it. */
#include <pthread.h>
int g;
void *t(void *arg)
{
  g = 1;
  return 0;
}
int main()
{
  pthread_t a;
  g = 0;
  pthread_create(&a, 0, t, 0);
  g = 2;
  pthread_join(a, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_race_join_fail
// Needs: --partial-order --ub-data-race-check
// RUN: --partial-order --ub-data-race-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
