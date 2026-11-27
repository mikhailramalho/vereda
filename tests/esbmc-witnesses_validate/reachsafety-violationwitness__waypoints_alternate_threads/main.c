#include <assert.h>
#include <pthread.h>

extern int __VERIFIER_nondet_int(void);

int g;

void *t(void *arg)
{
  int x = __VERIFIER_nondet_int();
  assert(x + g != 7);
  return 0;
}

int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  g = __VERIFIER_nondet_int();
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/waypoints_alternate_threads
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
