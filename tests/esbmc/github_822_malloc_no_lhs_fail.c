// A malloc whose result is discarded still allocates, and the storage is
// unreachable the moment the statement ends. goto-convert used to drop the
// call outright when there was no left-hand side, which made the leak
// invisible to --memory-leak-check. #822
#include <stdlib.h>

int main()
{
  malloc(10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_822_malloc_no_lhs_fail
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  dereference failure: forgotten memory: dynamic_1_array$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
