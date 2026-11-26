#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

char *v;
void *thread1(void * arg)
{
  v = calloc(8, sizeof(char));
  return 0;
}
void *thread2(void *arg)
{
  if (v) strcpy(v, "Bigshot");
  return 0;
}
int main()
{
  pthread_t t1, t2;
  pthread_create(&t1, 0, thread1, 0);
  pthread_create(&t2, 0, thread2, 0);
  pthread_join(t1, 0);
  pthread_join(t2, 0);
  assert(!v || v[0] == 'B');
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/github_1951
// Needs: --no-por --context-bound 2 --no-assertions --data-races-check
// RUN: --no-div-by-zero-check --force-malloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --32 --no-por --context-bound 2 --no-pointer-check --no-bounds-check --no-assertions --incremental-bmc --data-races-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
