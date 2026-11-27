/* #10856: __memset_impl's loop writes through a pointer, so its havoc must be
 * skipped, not only its entry assumption, when the loop is not entered. */
#include <string.h>

int main(void)
{
  char *p = 0;
  memset(p, 0, 0);
  unsigned x = 1;
  while (x != 0)
    x <<= 1;
  return 0;
}
// Imported from ESBMC regression/termination/github_10856_memset
// Needs: --termination --max-inductive-step 3
// RUN: --termination --ub-pointer-check=false --no-assertions --k-step 2 --max-k-step 40 --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Forward condition shows all executions terminate \(k = 33\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
