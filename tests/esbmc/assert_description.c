#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x > 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/assert_description
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
