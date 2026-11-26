#include <assert.h>

int main()
{
  unsigned n, i, s = 0;
  __VERIFIER_assume(n <= 3);

  for (i = 0; i < n; i++)
    s += 1;

  assert(s == n);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-showcex
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*FAILED)[\s\S]*\Z
// Vereda: verdict none
