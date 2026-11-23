/* The byte-reversed value the scatter used to store must not be readable. */
#include <assert.h>
#include <stdlib.h>

int main(void)
{
  int *v = (int *)malloc(sizeof(int) * 4);
  __VERIFIER_assume(v);
  v[0] = 0x01020304;
  assert(v[0] == 0x04030201);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7044-be-heap-int_fail
// Needs: --big-endian
// RUN: --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
