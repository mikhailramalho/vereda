#include <stdlib.h>

// A request of exactly K bytes must pass: the bound is `size <= K`, so K is
// allowed and only K+1 is flagged. Pins the off-by-one direction of the check.
int main(void)
{
  char *p = malloc(100);
  free(p);
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_excessive_alloc_boundary_ok
// Needs: --excessive-alloc-check=100
// RUN: --excessive-alloc-check=100 --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
