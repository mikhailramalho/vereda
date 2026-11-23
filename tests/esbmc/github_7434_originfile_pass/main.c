/* Regression for #7434, SUCCESSFUL arm: the same cross-file call with a
 * divisor that cannot be zero produces no violation and no witness trace. */
#include "helper.h"

int main(void)
{
  int d = 2;
  return helper_div(10, d);
}
// Imported from ESBMC regression/esbmc/github_7434_originfile_pass
// Needs: --witness-output-graphml -
// RUN: --witness-output-graphml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
