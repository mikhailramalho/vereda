// https://github.com/esbmc/esbmc/issues/2789
// C11 §6.5.7p3: shift by a negative amount is undefined behavior.
#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int a = __VERIFIER_nondet_int();
  assert((0 >> a) == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2789
// RUN: --overflow-check --no-unwinding-assertions --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
