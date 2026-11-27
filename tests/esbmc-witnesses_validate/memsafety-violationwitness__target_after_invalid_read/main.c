#include <stdlib.h>
int main(void)
{
  int *p = malloc(sizeof(*p));
  __VERIFIER_assume(p != 0);
  *p = 1;
  free(p);
  int value = *p;
  __VERIFIER_assume(value == 1);
  free(p);
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/target_after_invalid_read
// Needs: --validate-violation-witness --witness witness.yml --memory-leak-check
// RUN: -m32 --std c11 --validate-violation-witness --witness witness.yml --memory-leak-check --no-assertions --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
