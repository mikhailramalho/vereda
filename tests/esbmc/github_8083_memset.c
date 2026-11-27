#include <string.h>
#include <assert.h>

/* memset writes through its own pointer parameter, which the analysis cannot
   resolve, so it forgets everything. */
int main()
{
  int x = 5;
  memset(&x, 0, sizeof x);
  assert(x == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8083_memset
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
