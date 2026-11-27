#include <assert.h>

struct obj {
  int a;
  int b;
};

int main() {
  struct obj O;
  O.a = 4;
  assert(O.a == 4);
  __ESBMC_init_object(&O);
  __VERIFIER_assume(O.a == 4);
  assert(O.a == 4);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_2_true
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
