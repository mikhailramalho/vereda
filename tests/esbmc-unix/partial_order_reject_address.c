#include <pthread.h>
#include <assert.h>
int x;
void touch(int *p);
void *t(void *arg)
{
  touch(&x);
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_address
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support passing the address of a global to touch yet, at file .*partial_order_reject_address\.c line +7 column 3 function t$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
