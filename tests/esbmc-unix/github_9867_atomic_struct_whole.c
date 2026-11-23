// #9867: an _Atomic struct, or an array of them, assigned as a whole.
#include <pthread.h>
struct S { int f; };
typedef _Atomic struct S at_s;
_Atomic struct S s;
struct S _Atomic e;
at_s t;
_Atomic struct S arr[4];
struct S other;

void write_all(void)
{
  s = other;
  e = other;
  t = other;
  arr[1] = other;
}

void *thr(void *arg)
{
  write_all();
  return 0;
}

int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, thr, 0);
  write_all();
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9867_atomic_struct_whole
// Needs: --context-bound 2 --data-races-check-only
// RUN: --context-bound 2 --data-races-check-only --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
