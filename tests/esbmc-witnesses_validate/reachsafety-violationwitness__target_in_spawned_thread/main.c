#include <assert.h>
#include <pthread.h>

extern int __VERIFIER_nondet_int(void);

void *t(void *arg)
{
  int x = __VERIFIER_nondet_int();
  assert(x != 5);
  return 0;
}

int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/target_in_spawned_thread
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
