#include <assert.h>

int main() {
  char arr[10];
  assert(__ESBMC_r_ok(arr, 10));
}
// Imported from ESBMC regression/esbmc/r_ok5
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
