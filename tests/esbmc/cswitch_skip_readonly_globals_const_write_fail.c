#include <assert.h>
#include <pthread.h>
const int t[2] = {1, 2};
void *w(void *a)
{
  *(int *)&t[1] = 5;
  return 0;
}
int main()
{
  pthread_t a;
  pthread_create(&a, 0, w, 0);
  int x = t[1];
  int y = t[1];
  assert(x == y);
  return 0;
}
// Imported from ESBMC regression/esbmc/cswitch_skip_readonly_globals_const_write_fail
// Needs: --context-bound 3 --cswitch-skip-readonly-globals
// RUN: --context-bound 3 --cswitch-skip-readonly-globals
// MODE: FUTURE
// EXPECT: write access to const object `t'
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
