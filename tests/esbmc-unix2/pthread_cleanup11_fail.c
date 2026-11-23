#include <pthread.h>

void *thread_func(void *arg)
{
  // WARNING: This violates API contract – push/pop must match
  pthread_cleanup_pop(1); // undefined behavior – test how it's handled
  return NULL;
}

int main()
{
  pthread_t t;
  pthread_create(&t, NULL, thread_func, NULL);
  pthread_join(t, NULL);
  return 0;
}

// Imported from ESBMC regression/esbmc-unix2/pthread_cleanup11_fail
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
