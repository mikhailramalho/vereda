#include <string.h>
#include <assert.h>

/* A loop bound copied in by memcpy, through its pointer parameter. */
int main()
{
  int n = 3, m = __VERIFIER_nondet_int();
  memcpy(&n, &m, sizeof n);
  __VERIFIER_assume(n >= 0 && n < 6);
  int c = 0;
  for (int i = 0; i < n; i++)
    c++;
  assert(n < 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8083_memcpy
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
