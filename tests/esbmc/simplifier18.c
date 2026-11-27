#include <assert.h>

int main()
{
  int c = __VERIFIER_nondet_int();
  int d = __VERIFIER_nondet_int();
  int e = __VERIFIER_nondet_int();

  // (d - c) == (d - e) -> c == e
  assert(((d - c) == (d - e)) == (c == e));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier18
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
