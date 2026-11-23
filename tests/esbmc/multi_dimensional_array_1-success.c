#include <stdint.h>
int main()
{
  unsigned N, M;
  __VERIFIER_assume(N > 30);
  __VERIFIER_assume(M > 30);
  __VERIFIER_assume(M < 40);
  __VERIFIER_assume(N < SIZE_MAX / sizeof(int) / M);
  int arr[N][M];
  arr[15][25] = arr[10][15];
  // while(1)
  {
    __VERIFIER_assert(arr[15][25] == arr[10][15]); // This should be constant
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/multi_dimensional_array_1-success
// Needs: REQUIRES arch32
// RUN: --32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
