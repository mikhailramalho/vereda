// #9434: a TSD destructor still runs before pthread_join returns, whether
// the thread calls pthread_exit or returns.
#include <assert.h>
#include <pthread.h>

int __VERIFIER_nondet_int(void);

pthread_key_t k;
int v, done = 0;

void dtor(void *p)
{
  done = 1;
}

void *t(void *a)
{
  pthread_setspecific(k, &v);
  if (a)
    pthread_exit(0);
  return 0;
}

int main(void)
{
  pthread_t th;
  pthread_key_create(&k, dtor);
  pthread_create(&th, 0, t, (void *)(long)__VERIFIER_nondet_int());
  pthread_join(th, 0);
  assert(done == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9434
// RUN: -m64 --unwind 20
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
