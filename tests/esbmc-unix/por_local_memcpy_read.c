/* The worker reads main's local only through memcpy, which symex runs as an
 * intrinsic: MPOR must still key the read, or the schedule where main's
 * write lands before it is pruned (#7826). */
#include <assert.h>
#include <pthread.h>
#include <string.h>

void *worker(void *arg)
{
  int x;
  memcpy(&x, arg, sizeof(int));
  assert(x == 0 || x == 5);
  return 0;
}

int main()
{
  int done = 0;
  pthread_t t;
  pthread_create(&t, 0, worker, &done);
  done = 5;
  done = 0;
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/por_local_memcpy_read
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
