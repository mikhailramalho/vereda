#include <assert.h>
#include <stdlib.h>

static int cmp_int(const void *a, const void *b)
{
  int x = *(const int *)a, y = *(const int *)b;
  return (x > y) - (x < y);
}

int main(void)
{
  int a[4] = {3, 1, 4, 2};
  qsort(a, 4, sizeof a[0], cmp_int);
  int k = 4;
  assert(bsearch(&k, a, 4, sizeof a[0], cmp_int) == &a[3]);
  assert(a[0] == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/qsort_bsearch_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: PASSED\s+\[main.assertion.1\].*bsearch
// EXPECT: FAILED\s+\[main.assertion.2\].*a\[0\] == 3
// Vereda: unmet EXPECT: PASSED\s+\[main.assertion.1\].*bsearch
// MODE: FUTURE
// Vereda: verdict FAILED
