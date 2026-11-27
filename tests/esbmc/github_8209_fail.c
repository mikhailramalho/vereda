/* #8209: the wrapped join of [2, 3] and [3, 0] must keep 2. */
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
  assert(x != 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8209_fail
// Needs: --interval-analysis --interval-analysis-wrapped --interval-analysis-arithmetic
// RUN: --interval-analysis --interval-analysis-wrapped --interval-analysis-arithmetic --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
