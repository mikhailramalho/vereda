#include <assert.h>

int arr[10];

int main() {
  assert(arr[2] == 0);
  __ESBMC_init_object(arr);
  assert(arr[2] == 0);
}
// Imported from ESBMC regression/esbmc/builtin_init_var_4_false
// Needs: __ESBMC_init_object
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
