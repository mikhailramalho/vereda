#include <assert.h>
int __VERIFIER_nondet_int(void);
char shadow[1 << 20];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  if (i >= 0 && i < (1 << 20))
    shadow[i] = 1;
  assert(shadow[0] == 0);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/github_10938_ts_bmc_fail
// Needs: --ts-bmc
// RUN: --bitwuzla --ts-bmc
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)
// EXPECT: ^(VERIFICATION FAILED|ERROR: Solver error before the loop)$
// Vereda: verdict none
