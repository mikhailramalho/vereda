#include <assert.h>
#include <stdlib.h>

int main() {
  int N, M;  
  __VERIFIER_assume(N > 0 && N < 10);
  char *arr = malloc(N);
  __VERIFIER_assume(M > 0 && M < N);
  assert(__ESBMC_r_ok(arr, M));
}
// Imported from ESBMC regression/esbmc/r_ok3
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
