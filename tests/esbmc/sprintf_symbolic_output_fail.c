#include <stdio.h>

unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  char b[4];
  sprintf(b, "%x", __VERIFIER_nondet_uint());
  return 0;
}
// Imported from ESBMC regression/esbmc/sprintf_symbolic_output_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.1\]\s+line 8\s+dereference failure: array bounds violated$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
