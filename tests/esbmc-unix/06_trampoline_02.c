#include <pthread.h>
#include <assert.h>

int x;

void *thread1()
{
  if(x)
    assert(0);
  return NULL;
}

int main()
{
  pthread_t t1;
  pthread_create(&t1, 0, thread1, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/06_trampoline_02
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
