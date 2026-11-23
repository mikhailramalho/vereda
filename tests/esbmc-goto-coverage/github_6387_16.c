#include <assert.h>
int __VERIFIER_nondet_int();
int main()
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0 && n < 100);
  int s = 0;
  for (int i = 0; i < n; i++)
  {
    s += i;
    assert(s >= 0);
  }
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_16
// Needs: --branch-coverage
// RUN: --multi-property --branch-coverage --unwind 2
// MODE: FUTURE
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// EXPECT: reason: the unwinding bound cut off
// Vereda: verdict none
