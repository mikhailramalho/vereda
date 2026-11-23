/* #10111: a variable whose name contains '$' must be havocked by the
   inductive step like any other. */
#include <assert.h>

int main()
{
  unsigned c$ = 0;
  for (;;)
  {
    c$++;
    assert(c$ < 3);
  }
}
// Imported from ESBMC regression/k-induction/github_10111
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^Bug found \(k = 3\)$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
