// The success arm of C17 7.22.3p1 yields an object that must still be freed,
// so --memory-leak-check reports it exactly as it does without the option.
#include <stdlib.h>

int main(void)
{
  void *p = malloc(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5398_leak_fail
// Needs: --malloc-zero-is-null --memory-leak-check
// RUN: --force-malloc-success --malloc-zero-is-null --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*dereference failure: forgotten memory: dynamic_1_array$
// Vereda: verdict none
