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
// Imported from ESBMC regression/esbmc/github_4311-ascii
// Needs: --all-witnesses --ascii-report
// RUN: --all-witnesses --ascii-report --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  \+- Witness 1 of [0-9]+ -+$
// EXPECT: ^  \|  Inputs : \[0\] = -?[0-9]+$
// EXPECT: ^  \+-+$
// Vereda: verdict none
