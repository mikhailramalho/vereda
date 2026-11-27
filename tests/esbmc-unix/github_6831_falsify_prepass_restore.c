#include <assert.h>

int __VERIFIER_nondet_int();

int main()
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n == 5);

  int i = 0;
  while (i < n)
    i++;

  assert(i != 5);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_falsify_prepass_restore
// Needs: --falsify-context-bound 2
// RUN: --falsify-context-bound 2
// MODE: FUTURE
// EXPECT: ^Falsifying with context bound 2$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
