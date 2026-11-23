// Regression for github #5395: calloc of a zero-sized request must track
// malloc(0) under --malloc-zero-is-null too.  This pins the option-respecting
// boundary: asserting the pointer is non-null must FAIL here, because the fix
// routes calloc(_,0) through malloc(0), whose NULL alternative the option
// keeps reachable.
#include <stdlib.h>

void reach_error(void) {}
static void verifier_assert(int cond)
{
  if (!cond)
    reach_error();
}

int main(void)
{
  void *p = calloc(1, 0);
  verifier_assert(p != 0); // FAILS: calloc(1,0) may be NULL under the option
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_5395_calloc_zero_is_null_fail
// Needs: --malloc-zero-is-null
// RUN: --force-malloc-success --malloc-zero-is-null --no-pointer-check --no-bounds-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
