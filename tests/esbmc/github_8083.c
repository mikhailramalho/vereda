#include <stdio.h>
#include <assert.h>

int main()
{
  int x = 5;
  scanf("%d", &x);
  assert(x == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8083
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
