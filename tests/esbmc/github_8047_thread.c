// #8047: a thread switch re-parks paths without an interval snapshot, and the
// merge then sends the interval domain to top.
#include <pthread.h>

int g;

void *t(void *arg)
{
  g = -100;
  return 0;
}

int main()
{
  pthread_t th;
  int c;
  pthread_create(&th, 0, t, 0);
  if (c)
    ;
  else
    __VERIFIER_assume(g == 7);
  __VERIFIER_assert(g > -7); // g above -7
}
// Imported from ESBMC regression/esbmc/github_8047_thread
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
