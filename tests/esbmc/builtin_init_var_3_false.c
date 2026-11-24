#include <assert.h>

struct obj {
  int a;
  int b;
};

int main() {
  struct obj O[10];
  O[2].a = 4;
  assert(O[2].a == 4);
  __ESBMC_init_object(O);
  assert(O[2].a == 4);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_3_false
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
