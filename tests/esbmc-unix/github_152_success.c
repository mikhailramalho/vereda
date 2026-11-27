#include <assert.h>
#include <pthread.h>
void *t1(void *arg)
{
  //do something
  return NULL;
}
int main()
{
  pthread_t id;
  pthread_create(&id, NULL, t1, NULL);
  return 0;
  assert(0);
}
// Imported from ESBMC regression/esbmc-unix/github_152_success
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
