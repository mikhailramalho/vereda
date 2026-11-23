#include <assert.h>
#include <pthread.h>

int a, b;

int main(void)
{
  pthread_key_t k1, k2;
  pthread_key_create(&k1, 0);
  pthread_key_create(&k2, 0);
  pthread_setspecific(k1, &a);
  pthread_setspecific(k2, &b);
  assert(pthread_getspecific(k1) != &a);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9433_fail
// RUN: -m64 --unwind 20
// EXPECT: ^VERIFICATION FAILED$
