#include <stdlib.h>
#include <assert.h>

int main()
{
  int addr2 = __VERIFIER_nondet_int();
  __VERIFIER_assume(addr2 < 0);
  void *b = (void *)malloc(addr2);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_1631_nondet_compact
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION FAILED$
