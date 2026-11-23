#include <stdlib.h>

extern unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  // --force-malloc-success makes the initial 8-byte allocation non-NULL, so
  // the frontend's `p == 0 ? malloc(n) : realloc(p, n)` lowering takes the
  // realloc branch, where the attacker-controlled n is unbounded (CWE-789).
  char *p = malloc(8);
  unsigned n = __VERIFIER_nondet_uint();
  p = realloc(p, n);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_realloc
// Needs: --excessive-alloc-check
// RUN: --excessive-alloc-check --force-malloc-success --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  excessive allocation size: realloc$
// EXPECT: ^  CWE: CWE-789$
// Vereda: verdict none
