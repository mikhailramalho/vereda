// #9867: an atomic member and a plain member of a union share their bytes,
// so an atomic write still races with a plain write to the other member.
#include <pthread.h>
union U { _Atomic int f; int l; } u;

void *thr(void *arg)
{
  u.f = 1;
  return 0;
}

int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, thr, 0);
  u.l = 2;
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9867_atomic_union_mixed_fail
// Needs: --context-bound 2 --data-races-check-only
// RUN: --context-bound 2 --data-races-check-only --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: W/W data race on c:@u.l
// Vereda: verdict none
