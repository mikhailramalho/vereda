/* Verify that pthread_cancel delivers cancellation through cleanup handlers.
 * Thread cancels itself then hits a testcancel, guaranteeing cleanup runs. */
#include <pthread.h>
#include <assert.h>

int flag = 0;

void cleanup(void *arg)
{
  flag = 1;
}

void *thread_func(void *arg)
{
  pthread_cleanup_push(cleanup, NULL);
  pthread_cancel(pthread_self()); /* guaranteed before testcancel */
  pthread_testcancel();
  pthread_cleanup_pop(0);
  return NULL;
}

int main()
{
  pthread_t t;
  pthread_create(&t, NULL, thread_func, NULL);
  pthread_join(t, NULL);
  assert(flag == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/pthread_cleanup11
// Needs: --context-bound 3
// RUN: --context-bound 3 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
