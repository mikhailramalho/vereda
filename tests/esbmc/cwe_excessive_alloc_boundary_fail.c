#include <stdlib.h>

// A request of 100 bytes against a bound of 99 exceeds K by one byte and must
// be flagged. Together with cwe_excessive_alloc_boundary_ok this pins the
// exact `size <= K` boundary against a silent off-by-one regression.
int main(void)
{
  char *p = malloc(100);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_boundary_fail
// Needs: --excessive-alloc-check=99
// RUN: --excessive-alloc-check=99 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  excessive allocation size: malloc$
// EXPECT: ^  CWE: CWE-789$
// Vereda: verdict none
