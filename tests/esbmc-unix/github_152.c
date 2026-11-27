#include <assert.h>
#include <pthread.h>
void *t1(void *arg)
{
  int *ptr = arg;
  assert(*ptr == 1);
  return 0;
}
int main()
{
  int *p, incr = 0;
  pthread_t id;
  incr++;
  p = &incr;
  pthread_create(&id, NULL, t1, &incr);
  pthread_join(id, NULL);
  assert(*p == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_152
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
