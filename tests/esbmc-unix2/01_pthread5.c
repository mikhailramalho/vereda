#include <pthread.h>

void __VERIFIER_atomic_begin();
void __VERIFIER_atomic_end();

int g, x;
_Bool lock=0;

void *thread1(void *arg)
{
  if(lock)
  {
    x=1;
    __VERIFIER_assert(g==1); // property
  }
}

int main(void)
{
  pthread_t id1, id2;
  
  pthread_create(&id1, NULL, thread1, NULL);

  g=1;
  lock=1;

  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread5
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
