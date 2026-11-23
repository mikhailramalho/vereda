// github #9361: the overflowing calloc path is reachable and yields NULL.
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

int main(void)
{
  size_t n = __VERIFIER_nondet_bool() ? 4 : SIZE_MAX / 2;
  int *p = calloc(n, sizeof(int));
  assert(n == 4);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9361_fail
// RUN: -m64 --unwind 20
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
