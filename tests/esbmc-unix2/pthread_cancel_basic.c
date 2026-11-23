/* Verify that pthread_cancel + pthread_testcancel complete without errors.
 * Result is NULL (cancel not delivered) or PTHREAD_CANCELED (delivered). */
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
  assert(result == NULL || result == PTHREAD_CANCELED);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/pthread_cancel_basic
// Needs: --context-bound 3
// RUN: --context-bound 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
