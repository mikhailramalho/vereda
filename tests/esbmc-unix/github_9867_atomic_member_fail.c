// #9867: a plain member next to an _Atomic one still races.
#include <pthread.h>
struct S { _Atomic int f; int g; };
struct S s;

void *thr(void *arg)
{
  s.f = 1;
  s.g = 1;
  return 0;
}

int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, thr, 0);
  s.f = 2;
  s.g = 2;
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9867_atomic_member_fail
// Needs: --context-bound 2 --data-races-check-only
// RUN: --context-bound 2 --data-races-check-only --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: W/W data race on c:@s.g
// Vereda: verdict none
