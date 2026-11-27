#include <assert.h>

/* --restrict-assume must NOT prove this. A null pointer designates no object
 * and is never accessed here, so f(NULL, NULL) is a conforming call
 * (C11 6.7.3.1p4) and a != b does not follow from restrict. */
void f(void *restrict a, void *restrict b)
{
  assert(a != b);
}
// Imported from ESBMC regression/esbmc/restrict_assume_null_fail
// Needs: --restrict-assume
// RUN: --entry f --restrict-assume --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
