#include <pthread.h>
#include <assert.h>

int a,b;
void __VERIFIER_atomic_acquire(void)
{
  __VERIFIER_assume(a == 0);
  a = 1;
}
void* c(void *arg)
{
  __VERIFIER_atomic_acquire();
  b = 1;
  return NULL;
}
pthread_t d;
int main()
{
  pthread_create(&d, 0, c, 0);
  b = 1;
  __VERIFIER_atomic_acquire();
  if(b)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/github_402_fail2
// RUN:
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
