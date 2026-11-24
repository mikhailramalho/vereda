#include <assert.h>

int main() {
  int v;
  v = 0;

  assert(v == 0);
  __ESBMC_init_object(&v);
  assert(v == 0);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_0_false
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
