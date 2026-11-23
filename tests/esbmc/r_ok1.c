#include <assert.h>

int main() {
  char arr[10];
  assert(__ESBMC_r_ok(arr, 0));
}
// Imported from ESBMC regression/esbmc/r_ok1
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
