#include <assert.h>
#include <pthread.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint(void);

char d[4];
unsigned n;

void *writer(void *arg)
{
  memset(d, 1, n);
  return 0;
}

int main()
{
  n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 2 && n <= 4);
  pthread_t t;
  pthread_create(&t, 0, writer, 0);
  char a = d[0];
  char b = d[1];
  assert(!(a == 1 && b == 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10991_torn_fail
// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
