#include <stdlib.h>

int main(void)
{
  void *p = malloc(0);
  free(p);

  /* malloc(0) is NULL under this flag, so asserting otherwise must be
   * reported -- if p were unconstrained again this would silently pass. */
  __VERIFIER_assert(p != 0); // malloc(0) is not NULL
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6593_fail
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
