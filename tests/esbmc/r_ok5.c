#include <assert.h>

int main() {
  char arr[10];
  assert(__ESBMC_r_ok(arr, 10));
}
// Imported from ESBMC regression/esbmc/r_ok5
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
