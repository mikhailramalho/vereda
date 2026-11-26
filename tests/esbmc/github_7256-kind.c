#include <assert.h>

int main()
{
  unsigned n, i, s = 0;
  __VERIFIER_assume(n <= 3);

  for (i = 0; i < n; i++)
    s += 1;

  assert(s != 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7256-kind
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^Properties: 2 checked, \d+ passed, 1 failed, \d+ not checked, 1 not checked$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*produced no model)[\s\S]*\Z
// Vereda: verdict none
