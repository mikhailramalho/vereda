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
  {
    pthread_create(&c, 0, e, 0);
    return 0;
  }
  else
  {
    d = 3;
    assert(d == 3);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_397_1
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
