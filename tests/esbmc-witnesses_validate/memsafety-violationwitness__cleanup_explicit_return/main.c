#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int *p = malloc(sizeof(int));
  __VERIFIER_assume(p != 0);
  if (__VERIFIER_nondet_int())
  {
    free(p);
    return 0;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/cleanup_explicit_return
// Needs: --memory-leak-check --validate-violation-witness --witness witness.yml
// RUN: --memory-leak-check --validate-violation-witness --witness witness.yml --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
