#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_uint counter = 0;

void *increment(void *arg)
{
  counter = counter + 1; /* CK_AtomicToNonAtomic (read) + CK_NonAtomicToAtomic (write) */
  return NULL;
}

int main()
{
  pthread_t t1, t2;
  pthread_create(&t1, NULL, increment, NULL);
  pthread_create(&t2, NULL, increment, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  assert(counter > 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2149_pthread
// Needs: --context-bound 2 --ub-data-race-check
// RUN: --context-bound 2 --ub-data-race-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
