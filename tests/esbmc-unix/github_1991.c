// #1991: creating a thread on one branch must not hide the assertions on the
// path where it was never created.
#include <assert.h>
#include <pthread.h>

_Bool __VERIFIER_nondet_bool(void);

pthread_t c;
int d;

void *e(void *arg)
{
  d = 6;
  return 0;
}

int main()
{
  if (__VERIFIER_nondet_bool())
  {
    pthread_create(&c, 0, e, 0);
    return 0;
  }
  d = 3;
  assert(d == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_1991
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
