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
  pthread_join(th, 0);
  __VERIFIER_assert(a[0] == 1 && a[1] == 1); // written
  return 0;
}
// Imported from ESBMC regression/esbmc/array_loop_summary_threads
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
