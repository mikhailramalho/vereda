/* Two threads each write a nondet value in [1, 100] to an atomic_uint.
 * Regardless of which thread wins, the final value must stay in range. */
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>

unsigned int __VERIFIER_nondet_uint();

atomic_uint counter = 0;

void *assign(void *arg)
{
  unsigned int v = __VERIFIER_nondet_uint();
  __VERIFIER_assume(v >= 1 && v <= 100);
  counter = v; /* CK_NonAtomicToAtomic */
  return NULL;
}

int main()
{
  pthread_t t1, t2;
  pthread_create(&t1, NULL, assign, NULL);
  pthread_create(&t2, NULL, assign, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  assert(counter >= 1 && counter <= 100);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2149_pthread_3
// Needs: --context-bound 2 --data-race-check
// RUN: --context-bound 2 --data-race-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
