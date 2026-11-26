#include <assert.h>
unsigned int __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned int i, s = 0;
  for (i = 0; i < n; i++)
    s = s + 1;
  assert(s == n);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7256-showcex-trace
// Needs: --k-induction
// RUN: --k-induction --max-k-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// EXPECT: ^Violated property:$
// EXPECT: \A(?![\s\S]*FAILED)[\s\S]*\Z
// Vereda: verdict none
