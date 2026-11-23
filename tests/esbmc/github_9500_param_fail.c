#include <assert.h>

void rec(int n) {
  if (n == 0)
    return;
  rec(n - 1);
  assert(n != 2);
}

int main(void) {
  rec(3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9500_param_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --unwind 4
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
