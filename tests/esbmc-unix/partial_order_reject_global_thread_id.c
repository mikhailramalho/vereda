#include <pthread.h>
#include <assert.h>
pthread_t id;
void *t(void *arg)
{
  return 0;
}
int main()
{
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_global_thread_id
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a thread id stored in a global yet, at file .*partial_order_reject_global_thread_id\.c line +10
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
