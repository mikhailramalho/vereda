#include <pthread.h>

int g;

void __VERIFIER_atomic_begin();
void __VERIFIER_atomic_end();

void *t1(void *arg)
{
  // this is atomic!  
  __VERIFIER_atomic_begin();
  g=2;
  g=1;
  __VERIFIER_atomic_end();
}

int main()
{
  g=1;

  pthread_t id1;

  pthread_create(&id1, NULL, t1, NULL);
    
  assert(g==1);
}

// Imported from ESBMC regression/esbmc-unix2/01_pthread47
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
