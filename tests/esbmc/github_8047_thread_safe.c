// #8047: a thread switch re-parks paths without an interval snapshot; the
// merge must then widen the domain to top rather than keep this path's ranges.
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
  __VERIFIER_assert(g == 0 || g == 7 || g == -100); // g in range
}
// Imported from ESBMC regression/esbmc/github_8047_thread_safe
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
