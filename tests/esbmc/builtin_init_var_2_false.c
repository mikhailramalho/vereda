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
  assert(O.a == 4);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_2_false
// Needs: __ESBMC_init_object
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
