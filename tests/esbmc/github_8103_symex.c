#include <assert.h>

/* #8103: the wrapped interval domain must not build an interval from a float
 * operand of a cast or a comparison. */
int main(void)
{
  float f = __VERIFIER_nondet_float();
  __VERIFIER_assume(f > 2.5f && f < 3.5f);
  int i = (int)f;
  if (f > 1.0f)
    assert(i == 2 || i == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8103_symex
// Needs: --interval-analysis-wrapped
// RUN: --interval-analysis-wrapped
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
