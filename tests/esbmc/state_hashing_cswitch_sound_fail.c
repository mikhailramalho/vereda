#include <pthread.h>
#include <assert.h>

int a;

void *W0(void *arg)
{
  a = 1;
  if (a == 0)
    assert(0);
  return 0;
}

void *W1(void *arg)
{
  a = 0;
  a = 2;
  return 0;
}

int main()
{
  pthread_t t0, t1;
  pthread_create(&t0, 0, W0, 0);
  pthread_create(&t1, 0, W1, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/state_hashing_cswitch_sound_fail
// Needs: --no-por --context-bound 3
// RUN: --no-por --context-bound 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
