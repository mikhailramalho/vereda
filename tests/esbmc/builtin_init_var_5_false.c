#include <assert.h>

struct obj {
  int a;
  int b;
};

struct obj arr[10];

int main() {
  assert(arr[2].b == 0);
  __ESBMC_init_object(arr);
  assert(arr[2].b == 0);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_5_false
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
