#include <assert.h>
#include <pthread.h>
#include <threads.h>

int main(void)
{
  pthread_key_t k;
  if (pthread_key_create(&k, 0) != 0)
    return 0;
  assert(pthread_setspecific(k, 0) == 0);
  assert(pthread_getspecific(k) == 0);

  tss_t t;
  if (tss_create(&t, 0) != thrd_success)
    return 0;
  assert(tss_set(t, 0) == thrd_success);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9442
// RUN: -m64 --unwind 20
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
