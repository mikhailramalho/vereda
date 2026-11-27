/* esbmc/esbmc#8681: a guarded index into the free operand passes its checks. */
#include <stdlib.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int *a[2] = {0, malloc(sizeof(int))};
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k == 1 || k == 2);
  free(a[2 / (k + 1)]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8681
// RUN: --multi-property
// EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.1\]\s+line\s+9\s
// EXPECT: ^\s+PASSED\s+\[main\.division-by-zero\.1\]\s+line\s+9\s
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.1\]\s+line\s+9\s
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
