#include <assert.h>

int main()
{
  _Bool x, y;
  int a, b, f;
  x = __VERIFIER_nondet_bool();
  y = __VERIFIER_nondet_bool();
  a = ((2 * x) - (3 * y));
  if(a < 0)
    a = 0;
  b = (x + (4 * y));
  if(b < 0)
    b = 0;
  f = ((3 * x) + y);
  if(f < 0)
    f = 0;
  assert(a <= 2 && b <= 5 && f <= 4);
  return 0;
}
// Imported from ESBMC regression/incremental-smt/incremental-01
// Needs: --smt-during-symex --smt-symex-guard
// RUN: --smt-during-symex --smt-symex-guard
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
