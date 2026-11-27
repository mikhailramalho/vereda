// --malloc-zero-is-null is a statement about malloc: alloca has no NULL
// alternative to explore, so a zero-sized frame allocation is unaffected.
#include <stdlib.h>

int main(void)
{
  void *p = __builtin_alloca(0);
  __VERIFIER_assert(p != 0); // alloca(0) is not NULL
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5398_alloca
// Needs: --malloc-zero-is-null
// RUN: --malloc-zero-is-null --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
