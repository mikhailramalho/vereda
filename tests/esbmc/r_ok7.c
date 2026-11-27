#include <assert.h>
#include <stdlib.h>

int main() {
  int N, M;  
  char *arr = malloc(N);
  assert(__ESBMC_r_ok(arr, M));
}
// Imported from ESBMC regression/esbmc/r_ok7
// Needs: __ESBMC_r_ok
// RUN: --force-malloc-success --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
