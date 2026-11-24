#include <assert.h>

int main() {
  char arr[10];
  assert(__ESBMC_r_ok(arr, 0));
}
// Imported from ESBMC regression/esbmc/r_ok1
// Needs: __ESBMC_r_ok
// RUN: --no-pointer-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
