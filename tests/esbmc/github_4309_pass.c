#include <assert.h>

int main(void)
{
  int x;
  if (x > 0)
    x--;
  else
    x++;
  // Trivially true: no enumeration should occur on the UNSAT path.
  assert(x == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4309_pass
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
