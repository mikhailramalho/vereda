/* #10990: a read and a write of g in different threads race. */
#include <pthread.h>
int g;
void *reader(void *arg)
{
  int l = g;
  return 0;
}
void *writer(void *arg)
{
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
// Imported from ESBMC regression/esbmc-unix/partial_order_race_fail
// Needs: --partial-order --ub-data-race-check
// RUN: --partial-order --ub-data-race-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: R/W data race on c:@g
// Vereda: verdict none
