#include <assert.h>

int main(void)
{
  int x;
  if (x > 0)
    x--;
  else
    x++;
  assert(x != 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4309
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[Counterexamples - 2 witnesses\]
// EXPECT: Witness 1 of 2
// EXPECT: Witness 2 of 2
// EXPECT: Inputs : \[0\] = -1
// EXPECT: Inputs : \[0\] = 1
// EXPECT: Summary: 2 distinct input tuples violate this property
// EXPECT: UNSAT after 2 witnesses
// Vereda: verdict none
