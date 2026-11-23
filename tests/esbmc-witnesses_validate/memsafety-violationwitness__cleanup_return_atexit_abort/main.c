#include <stdlib.h>
static void *p;
static void cleanup(void) { abort(); }
int main(void)
{
  p = malloc(4);
  __VERIFIER_assume(p != 0);
  atexit(cleanup);
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/cleanup_return_atexit_abort
// Needs: --memory-leak-check --validate-violation-witness --witness witness.yml
// RUN: --memory-leak-check --validate-violation-witness --witness witness.yml --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
