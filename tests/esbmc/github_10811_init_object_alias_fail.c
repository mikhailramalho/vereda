#include <assert.h>
int a[2] = {1, 2};
int b[2];
int main()
{
  int *p = __VERIFIER_nondet_bool() ? a : b;
  __VERIFIER_assume(p == b);
  __ESBMC_init_object(p);
  assert(b[0] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10811_init_object_alias_fail
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
