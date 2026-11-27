/* #10990: the trace shows t's write before main reads it. */
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
  int m = x;
  assert(m == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_trace_write_fail
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?=[\s\S]*thread 1\n  x = 1 [\s\S]*\n  m = 1 [\s\S]*Violated property)
// Vereda: verdict none
