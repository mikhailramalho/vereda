#include <assert.h>
#include <stdio.h>
int a[2] = {1, 2};
int b[2];
int main()
{
  int *p = __VERIFIER_nondet_bool() ? a : b;
  __VERIFIER_assume(p == b);
  sscanf("5", "%d", p);
  assert(b[0] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10811_scanf_alias_fail
// EXPECT: ^VERIFICATION FAILED$
