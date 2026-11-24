#include <assert.h>
#include <stdlib.h>

int main() {
  int N, M;  
  char *arr = malloc(__VERIFIER_nondet_int() ? N : N-1);
  __VERIFIER_assume(N > 0 && M < N); 
  assert(__ESBMC_r_ok(arr, M));
}
// Imported from ESBMC regression/esbmc/r_ok8
// Needs: __ESBMC_r_ok
// RUN: --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
