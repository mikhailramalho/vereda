#include <assert.h>
#include <stdio.h>
int x = 0;
int main()
{
  void *p = &x;
  int r = sscanf("0 5", "%p%d", &p, (int *)p);
  if (r == 2)
    assert(x == 0 || p != (void *)0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10811_scanf_chain_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
