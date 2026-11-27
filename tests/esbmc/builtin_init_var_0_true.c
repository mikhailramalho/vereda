#include <assert.h>

int main() {
  int v;
  v = 0;

  assert(v == 0);
  __ESBMC_init_object(&v);
  __VERIFIER_assume(v > 0);
  assert(v > 0);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_0_true
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
