#include <stdlib.h>

extern unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  // Attacker-controlled, unbounded allocation size (CWE-789): n can be up to
  // UINT_MAX, well above the default 1 MiB --excessive-alloc-check bound.
  unsigned n = __VERIFIER_nondet_uint();
  char *p = malloc(n);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc
// Needs: --excessive-alloc-check
// RUN: --excessive-alloc-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  excessive allocation size: malloc$
// EXPECT: ^  CWE: CWE-789$
// Vereda: verdict none
