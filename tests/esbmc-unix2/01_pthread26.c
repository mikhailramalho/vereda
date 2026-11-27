#include <pthread.h>

void *t1(void *arg)
{
  _Bool local_flag;
  
  local_flag=0;
  local_flag=1;
  assert(local_flag);
}

int main()
{
  pthread_t id1;

  pthread_create(&id1, NULL, t1, NULL);
}

// Imported from ESBMC regression/esbmc-unix2/01_pthread26
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
