#include <assert.h>

int main() {
  int v[10];
  v[9] = 5;

  assert(v[9] == 5);
  __ESBMC_init_object(v);
  __VERIFIER_assume(v[9] == 5);
  assert(v[9] == 5);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_1_true
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
