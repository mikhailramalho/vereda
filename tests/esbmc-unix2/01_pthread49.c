#include <assert.h>
#include <pthread.h>

int main()
{
  pthread_key_t key;
  int r = pthread_key_create(&key, NULL);
  assert(r == 1); /* r must be 0 */
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread49
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
