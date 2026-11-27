// --interval-analysis simplifies the unused libm models <math.h> pulls in,
// whose remainder() call must not become a floating-point-only node.
#include <math.h>
int main()
{
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10774_interval_fixedbv
// Needs: --interval-analysis
// RUN: --interval-analysis --ub-div-by-zero-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
