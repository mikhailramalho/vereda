#include <assert.h>
int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  char a[n];
  unsigned k = sizeof(a);
  assert(n == k);
}
// Imported from ESBMC regression/esbmc/github_588
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
