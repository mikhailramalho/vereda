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
// Imported from ESBMC regression/bitwuzla/github_10938_ts_pdr_fail
// Needs: --ts-pdr
// RUN: --bitwuzla --ts-pdr
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)
// EXPECT: ^VERIFICATION (FAILED|UNKNOWN)$
// Vereda: verdict none
