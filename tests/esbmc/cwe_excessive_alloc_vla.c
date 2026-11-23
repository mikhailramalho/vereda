#include <stdlib.h>

extern unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  // sizeof(int[n]) is a VLA type: the frontend records element count 1 with a
  // dynamically-sized array element type, so type_byte_size throws and the
  // check must fall back to the symbolic byte size (n * sizeof(int)) rather
  // than skip the allocation. Unbounded n can exceed the 1 MiB bound (CWE-789).
  unsigned n = __VERIFIER_nondet_uint();
  int *p = malloc(sizeof(int[n]));
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_vla
// Needs: --excessive-alloc-check
// RUN: --excessive-alloc-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  excessive allocation size: malloc$
// EXPECT: ^  CWE: CWE-789$
// Vereda: verdict none
