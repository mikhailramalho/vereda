#include <threads.h>

/* #8189: t1 can take the lock once main releases it, while t2 writes. */
int global;
mtx_t m;
thrd_t id1, id2;

int t1(void *arg)
{
  mtx_lock(&m);
  global++;
  mtx_unlock(&m);
  return 0;
}

int t2(void *arg)
{
  global++;
  return 0;
}

int main()
{
  mtx_init(&m, mtx_plain);
  thrd_create(&id1, t1, 0);
  mtx_lock(&m);
  thrd_create(&id2, t2, 0);
  mtx_unlock(&m);
  thrd_join(id2, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/race_c11_mtx_unlock_then_join_fail
// Needs: --data-races-check-only
// RUN: --data-races-check-only --no-assertions --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: W/W data race on c:@global
// Vereda: verdict none
