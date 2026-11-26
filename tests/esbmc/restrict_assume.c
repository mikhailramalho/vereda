#include <assert.h>
void f(void *restrict a, void *restrict b)
{
  __VERIFIER_assume(a != 0);
  __VERIFIER_assume(b != 0);
  assert(a != b);
}
// Imported from ESBMC regression/esbmc/restrict_assume
// Needs: --entry f --restrict-assume
// RUN: --entry f --restrict-assume
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
