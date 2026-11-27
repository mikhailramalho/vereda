#include <stdlib.h>
int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n < 4);
  int (*p)[n] = malloc(4 * n * sizeof(int));
  if (!p)
    return 0;
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(j < 4);
  p[j][0] = 7;
  __VERIFIER_assert(p[j][0] == 7); // vla row
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_arith_vla_row
// EXPECT: ^VERIFICATION SUCCESSFUL$
