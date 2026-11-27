/* VERIFICATION FAILED: asserts cancellation is never delivered, but ESBMC
 * must find the interleaving where pthread_cancel delivers PTHREAD_CANCELED. */
#include <pthread.h>
#include <assert.h>

void *thread_func(void *arg)
{
  pthread_testcancel();
  return NULL;
}

int main()
{
  pthread_t t;
  void *result;
  pthread_create(&t, NULL, thread_func, NULL);
  pthread_cancel(t);
  pthread_join(t, &result);
  assert(result != PTHREAD_CANCELED); /* must fail: cancel CAN be delivered */
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/pthread_cancel_basic_fail
// Needs: --context-bound 3
// RUN: --context-bound 3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
