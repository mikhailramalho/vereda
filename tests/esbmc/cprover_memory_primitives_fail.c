#include <stdlib.h>

int main(void)
{
  char *p = malloc(8);
  if (!p)
    return 0;
  __VERIFIER_assert(__CPROVER_OBJECT_SIZE(p) == 16); // wrong size on purpose
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cprover_memory_primitives_fail
// Needs: __CPROVER_OBJECT_SIZE
// RUN: --no-pointer-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
