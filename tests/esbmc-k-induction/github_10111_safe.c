/* #10111 */
#include <assert.h>

int main()
{
  unsigned c$ = 0;
  for (;;)
  {
    c$ = (c$ + 1) % 3;
    assert(c$ < 3);
  }
}
// Imported from ESBMC regression/k-induction/github_10111_safe
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = \d+\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
