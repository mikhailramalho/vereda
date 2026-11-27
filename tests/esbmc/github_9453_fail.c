#include <assert.h>

int main(void)
{
  _Bool b = 1;
  __VERIFIER_assume(b != 0);
  b ^= 1;
  assert(b == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9453_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
