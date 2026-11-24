#include <pthread.h>

int g;

void *t1(void *arg)
{
  pthread_exit(0);
  g=1;
}

int main()
{
  pthread_t id1;
  
  int arg1=10;
  
  g=10;

  pthread_create(&id1, NULL, t1, &arg1);
  
  assert(g==10);
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread4
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
