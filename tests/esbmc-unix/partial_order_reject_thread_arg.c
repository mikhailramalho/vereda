#include <pthread.h>
#include <assert.h>
void *t(void *arg)
{
  *(int *)arg = 1;
  return 0;
}
int main()
{
  int v = 0;
  pthread_t id;
  pthread_create(&id, 0, t, &v);
  assert(v == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_thread_arg
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a thread argument other than NULL yet, at file .*partial_order_reject_thread_arg\.c line +12 column 3 function
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
