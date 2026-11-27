/* --scalarize-structs (#7521) leaves a program that may create threads alone:
 * a whole-struct copy split into one write per field would let another thread
 * see it half done. */
#include <pthread.h>

struct pair
{
  int x;
  int y;
};

struct pair shared;

void *writer(void *arg)
{
  shared.x = 1;
  shared.y = 1;
  return 0;
}

int main(void)
{
  pthread_t t;
  pthread_create(&t, 0, writer, 0);
  struct pair seen = shared;
  __VERIFIER_assert(seen.x == seen.y || seen.x == 1); // x is written first
  return 0;
}
// Imported from ESBMC regression/esbmc/scalarize_structs_threads
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^WARNING: --scalarize-structs is ignored: the program may create threads$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict UNKNOWN
