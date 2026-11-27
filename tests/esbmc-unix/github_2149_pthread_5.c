/* Two threads each write a constrained nondet signed value to an atomic_int.
 * Covers CK_NonAtomicToAtomic for a signed type; post-join read via
 * CK_AtomicToNonAtomic in main verifies the value stays in range. */
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>

int __VERIFIER_nondet_int();

atomic_int shared = 0;

void *assign(void *arg)
{
  int v = __VERIFIER_nondet_int();
  __VERIFIER_assume(v >= -10 && v <= 10);
  shared = v; /* CK_NonAtomicToAtomic */
  return NULL;
}

int main()
{
  pthread_t t1, t2;
  pthread_create(&t1, NULL, assign, NULL);
  pthread_create(&t2, NULL, assign, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  int result = shared; /* CK_AtomicToNonAtomic */
  assert(result >= -10 && result <= 10);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2149_pthread_5
// Needs: --context-bound 2 --ub-data-race-check
// RUN: --context-bound 2 --ub-data-race-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
