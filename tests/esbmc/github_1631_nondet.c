#include <stdlib.h>
#include <assert.h>

int main()
{
  int addr1 = __VERIFIER_nondet_int();
  __VERIFIER_assume(addr1 == 4);
  int addr2 = __VERIFIER_nondet_int();

  void *a = (void *)malloc(addr1);
  void *b = (void *)malloc(addr2);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_1631_nondet
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
