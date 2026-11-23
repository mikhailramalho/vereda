#include <pthread.h>
#include <assert.h>
int r;
void *t(void *arg)
{
  int i;
  for (i = 0; i < 10; i++)
    ;
  r = i;
  return 0;
}
int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  assert(r == 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8712_local_counter_simplified
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSIGN i=10
// Vereda: verdict none
