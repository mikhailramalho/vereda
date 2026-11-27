// A symbolic enclosing row index carries no constant offset, so the
// linearisation has to bail and leave &a[i][1] in its single-level form:
// folding it would read a nondet index as a constant (#6778).
#include <assert.h>

int main(void)
{
  int a[2][3] = {{1, 2, 3}, {4, 5, 6}};
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i <= 1);

  int *p = &a[0][1];

  assert((p == &a[i][1]) == (i == 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/addressof_multidim_symbolic_row
// Needs: --no-pointer-arith-check
// RUN: --no-pointer-arith-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
