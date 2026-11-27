#include <pthread.h>
#include <assert.h>

int x1 = 1;
int x3 = 1;

_Bool flag1=0;

void __VERIFIER_atomic_begin();
void __VERIFIER_atomic_end();

void *t1(void *arg)
{
  __VERIFIER_atomic_begin();
    x1 = (x3+1)%4;
  flag1=1;
  __VERIFIER_atomic_end();
}

int main()
{
  pthread_t id1;
  void *thread_result;

  pthread_create(&id1, NULL, t1, NULL);

  pthread_join(id1, &thread_result);

  __VERIFIER_atomic_begin();
  if (flag1)
    assert(x1 != x3);
  __VERIFIER_atomic_end();

  return 0;
}

// Imported from ESBMC regression/esbmc-unix2/01_pthread33
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
