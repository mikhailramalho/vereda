#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

int main()
{
  pthread_key_t key;
  int r = pthread_key_create(__VERIFIER_nondet_bool() ? &key : NULL, NULL);
  if(r == ENOMEM)
  {
    exit(1);
  }
  assert(r == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread51
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
