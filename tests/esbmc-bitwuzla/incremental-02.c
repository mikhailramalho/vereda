#include "assert.h"

int main()
{
  int x = 1;
  int y = 0;
  while(y < 10 && __VERIFIER_nondet_int())
  {
    x = x + y;
    y = y + 1;
  }
  assert(x >= y);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/incremental-02
// Needs: --smt-symex-guard
// RUN: --smt-symex-guard --bitwuzla
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
