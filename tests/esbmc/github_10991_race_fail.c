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
  __VERIFIER_assume(n >= 1 && n <= 4);
  pthread_t t;
  pthread_create(&t, 0, writer, 0);
  char a = d[0];
  pthread_join(t, 0);
  return a;
}
// Imported from ESBMC regression/esbmc/github_10991_race_fail
// Needs: --data-race-check
// RUN: --data-race-check --unwind 6
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  R/W data race on c:@d$
// Vereda: verdict none
