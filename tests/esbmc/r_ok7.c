#include <assert.h>
#include <stdlib.h>

int main() {
  int N, M;  
  char *arr = malloc(N);
  assert(__ESBMC_r_ok(arr, M));
}
// Imported from ESBMC regression/esbmc/r_ok7
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION FAILED$
