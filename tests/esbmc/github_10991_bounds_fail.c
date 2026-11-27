#include <string.h>

unsigned __VERIFIER_nondet_uint(void);

int main()
{
  char d[8];
  unsigned off = __VERIFIER_nondet_uint(), n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(off <= 8 && n <= 9 - off);
  memset(d + off, 0, n);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10991_bounds_fail
// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: memset length exceeds object bounds$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
