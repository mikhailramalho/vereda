#include <assert.h>
#include <pthread.h>

_Bool receive = 0;
int n = 0;
int dummy = 0;

void *t1(void *arg)
{
  if (__VERIFIER_nondet_int())
  {
    if (receive)
      receive = 0;
  }
  assert(!receive || n);
  return NULL;
}

int main()
{
  pthread_t id;
  n = __VERIFIER_nondet_int() != 0;
  pthread_create(&id, NULL, t1, NULL);
  if (n)
    receive = 1;
  if (dummy)
    dummy = 0;
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/race_guard_nested_cond
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
