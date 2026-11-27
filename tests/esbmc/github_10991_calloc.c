#include <assert.h>
#include <stdlib.h>

unsigned long __VERIFIER_nondet_ulong(void);

int main()
{
  unsigned long n = __VERIFIER_nondet_ulong();
  unsigned long k = __VERIFIER_nondet_ulong();
  char *p = calloc(n, 4);
  if (p && k < 4 * n)
    assert(p[k] == 0);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10991_calloc
// EXPECT: ^VERIFICATION SUCCESSFUL$
