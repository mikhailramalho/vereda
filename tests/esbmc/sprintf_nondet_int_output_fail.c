#include <stdio.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 10 && x <= 99);
  char d[2];
  sprintf(d, "%d", x);
  return 0;
}
// Imported from ESBMC regression/esbmc/sprintf_nondet_int_output_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.1\]\s+line 10\s+dereference failure: array bounds violated$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
