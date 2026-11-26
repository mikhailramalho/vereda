/* Negative variant of github_4441 — boundary pin.
 *
 * Same body as github_4441 but without the __assert_fail in reach_error,
 * so control really does reach end-of-main with the heap object
 * unreachable.  The memcleanup check must still fire here, otherwise
 * the fix has over-suppressed the "forgotten memory" claim.
 */

#include <stdlib.h>

int main(void)
{
  char *p = malloc(8);
  if (!p)
    return 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4441_fail
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-assertions --incremental-bmc --k-step 2 -m64
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*dereference failure: forgotten memory:
// MODE: FUTURE
// Vereda: verdict none
