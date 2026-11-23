// https://github.com/esbmc/esbmc/issues/2789
// Companion to github_2789: when the shift distance is constrained to the
// well-defined range [0, width), no UB is reported.
#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a >= 0 && a < 32);
  assert((0 >> a) == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2789_pass
// RUN: --overflow-check --unwind 1
// EXPECT: ^VERIFICATION SUCCESSFUL$
