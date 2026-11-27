/* esbmc/esbmc#8681: the operand of free is checked like any expression. */
#include <stdlib.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int *a[2] = {0, 0};
  int k = __VERIFIER_nondet_int();
  free(a[1 / k]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8681_fail
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.array-bounds-violated\.1\]\s+line\s+8\s
// EXPECT: ^\s+FAILED\s+\[main\.division-by-zero\.1\]\s+line\s+8\s
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+FAILED\s+\[main\.array-bounds-violated\.1\]\s+line\s+8\s
// MODE: FUTURE
// Vereda: verdict FAILED
