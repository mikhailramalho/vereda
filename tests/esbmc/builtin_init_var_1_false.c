#include <assert.h>

int main() {
  int v[10];
  v[9] = 5;

  assert(v[9] == 5);
  __ESBMC_init_object(v);
  assert(v[9] == 5);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_1_false
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
