#include <assert.h>
int g;

int f(int n) {
  g = 0;
  if (n <= 0)
    return 5;
  g = f(n - 1);
  assert(g == 0);
  g = 0;
  return 7;
}

int main(void) {
  f(1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9500_ret_global_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
