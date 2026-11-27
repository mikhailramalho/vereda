#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n == 3);

  int i = 0;
  while (i < n)
    i++;

  assert(i == n);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_falsify_prepass_partial
// Needs: --partial-loops --falsify-context-bound 1
// RUN: --partial-loops --falsify-context-bound 1 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Falsifying with context bound 1$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
