int main()
{
  unsigned N, M;
  __VERIFIER_assume(N > 30);
  __VERIFIER_assume(M > 30);
  int arr[N][M];
  arr[15][25] = arr[10][15];
  while(1)
  {
    __VERIFIER_assert(arr[15][25] == arr[10][15]); // This should be constant
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/multi_dimensional_array_1
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \barray size in bytes overflows address space size$
// Vereda: verdict none
