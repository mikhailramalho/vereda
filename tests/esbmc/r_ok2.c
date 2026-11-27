#include <assert.h>

int main() {
  int N, M;  
  char arr[N];
  __VERIFIER_assume(N > 0 && M > 0 && M < N); 
  assert(__ESBMC_r_ok(arr, M));
}
// Imported from ESBMC regression/esbmc/r_ok2
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
