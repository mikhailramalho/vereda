// A summary writes the whole array at once; main can still see it half
// written, so the pass leaves programs with threads alone.
#include <pthread.h>
int a[2];
void *t(void *arg)
{
  for (int i = 0; i < 2; i++)
    a[i] = 1;
  return 0;
}
int main()
{
  pthread_t th;
  pthread_create(&th, 0, t, 0);
  __VERIFIER_assert(!(a[0] == 1 && a[1] == 0)); // not half written
  return 0;
}
// Imported from ESBMC regression/esbmc/array_loop_summary_threads_fail
// RUN: --summarize-array-loops
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
