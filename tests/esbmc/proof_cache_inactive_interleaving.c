#include <assert.h>
#include <pthread.h>

int x;

void *inc(void *arg)
{
  x = x + 1;
  return 0;
}

int main(void)
{
  x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);

  pthread_t t;
  pthread_create(&t, 0, inc, 0);
  x = x + 1;
  pthread_join(t, 0);
  assert(x > 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_inactive_interleaving
// Needs: --proof-cache proof-store
// RUN: --multi-property --proof-cache proof-store
// MODE: FUTURE
// EXPECT: ^WARNING: Proof cache: inactive \(thread interleavings after the first, where a claim carries a schedule its cone does not name\); every claim will be solved and none stored$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
