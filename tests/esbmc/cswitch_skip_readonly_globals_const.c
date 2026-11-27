#include <assert.h>
#include <pthread.h>
const int t[2] = {1, 2};
int g;
void *w(void *a)
{
  for (int i = 0; i < 2; ++i)
    assert(t[i] == i + 1);
  return 0;
}
int main()
{
  const int *p = t;
  int *q = &g;
  *q = *p;
  pthread_t a, b;
  pthread_create(&a, 0, w, 0);
  pthread_create(&b, 0, w, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/cswitch_skip_readonly_globals_const
// Needs: --context-bound 3
// RUN: --context-bound 3 --no-incremental-bmc
// EXPECT: ^Thread interleavings 7$(?![\s\S]*Thread interleavings)
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
