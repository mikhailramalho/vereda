#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  __VERIFIER_assume(x == 0);
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  x = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_verify_assume
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support verifying an assumption yet, at file .*partial_order_reject_verify_assume\.c line +6
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
