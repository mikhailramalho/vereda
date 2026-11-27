/* #8209: the wrapped join of [2, 3] and [3, 0] is [2, 0], which excludes 1. */
#include <assert.h>
unsigned __VERIFIER_nondet_uint(void);
int __VERIFIER_nondet_int(void);

int main()
{
  unsigned y = __VERIFIER_nondet_uint();
  __VERIFIER_assume(y <= 0xFFFFFFFDu);
  unsigned x;
  if (__VERIFIER_nondet_int())
  {
    x = __VERIFIER_nondet_uint();
    __VERIFIER_assume(2 <= x && x <= 3);
  }
  else
    x = y + 3u;
  assert(x != 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8209
// Needs: --interval-analysis --interval-analysis-wrapped --interval-analysis-arithmetic --interval-analysis-dump
// RUN: --interval-analysis --interval-analysis-wrapped --interval-analysis-arithmetic --interval-analysis-dump --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^2 <= c:github_8209\.c@[0-9]+@F@main@x <= 0$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
