#include <pthread.h>
pthread_t c;
int d;
void *e(void *)
{
  d = 6;
}
int main()
{
  if(__VERIFIER_nondet_bool())
    pthread_create(&c, 0, e, 0);
  d = 3;
  assert(d == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_397_2
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
