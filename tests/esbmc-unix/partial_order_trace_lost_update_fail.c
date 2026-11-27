/* #10990: the trace shows both reads of x before either write. */
#include <pthread.h>
#include <assert.h>
int x;
void *t(void *arg)
{
  int tmp = x;
  x = tmp + 1;
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  int m = x;
  x = m + 1;
  pthread_join(id, 0);
  assert(x == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_trace_lost_update_fail
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?=[\s\S]*\n  tmp = 0 )(?=[\s\S]*\n  m = 0 )(?![\s\S]*\n  x = 1 [\s\S]*\n  (m|tmp) = 0 )
// Vereda: verdict none
