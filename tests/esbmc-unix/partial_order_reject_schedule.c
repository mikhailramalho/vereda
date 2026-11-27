#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  x = 1;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_schedule
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order cannot be combined with --schedule$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
