#include <assert.h>

int arr[10];

int main() {
  assert(arr[2] == 0);
  __ESBMC_init_object(arr);
  __VERIFIER_assume(arr[2] == 0);
  assert(arr[2] == 0);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_4_true
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
