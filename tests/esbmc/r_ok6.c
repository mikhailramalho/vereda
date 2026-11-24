#include <assert.h>

int main() {
  int N, M;  
  char arr[N];
  assert(__ESBMC_r_ok(arr, M));
}
// Imported from ESBMC regression/esbmc/r_ok6
// EXPECT: ^VERIFICATION FAILED$
