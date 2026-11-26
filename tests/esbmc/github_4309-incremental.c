#include <assert.h>

int main(void)
{
  int x;
  if (x > 0)
    x--;
  else
    x++;
  assert(x != 0);
  int y;
  if (y > 10)
    y--;
  else
    y++;
  assert(y != 11);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4309-incremental
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[Counterexamples - 2 witnesses\]
// EXPECT: Inputs : \[0\] = -1
// EXPECT: Inputs : \[0\] = 1
// EXPECT: Inputs : \[0\] = 10
// EXPECT: Inputs : \[0\] = 12
// Vereda: verdict none
