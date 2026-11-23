#include <stdlib.h>
static int depth;
int main(void)
{
  if (depth++ == 0)
  {
    void *p = malloc(4);
    __VERIFIER_assume(p != 0);
    main();
    return 0;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/cleanup_recursive_main_return
// Needs: --std c11 --memory-leak-check --validate-violation-witness --witness witness.yml
// RUN: --std c11 --memory-leak-check --validate-violation-witness --witness witness.yml --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
