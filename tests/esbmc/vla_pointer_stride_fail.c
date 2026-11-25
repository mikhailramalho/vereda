#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(m >= 1 && m <= 4);
  int a[3][m];
  a[2][m - 1] = 7;
  int (*p)[m] = a + 2;
  int (*q)[m] = a;
  assert(p - q == 2);
  assert((*p)[m - 1] == 8);
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_pointer_stride_fail
// RUN: --multi-property
// EXPECT: ^  PASSED .* dereference failure: array bounds violated$
// EXPECT: ^Properties: [0-9]+ checked, \d+ passed, 1 failed, \d+ not checked
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  PASSED .* dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict FAILED
