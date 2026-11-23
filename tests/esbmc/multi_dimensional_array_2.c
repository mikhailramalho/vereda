int main()
{
  unsigned N, M, O;
  __VERIFIER_assume(N > 30);
  __VERIFIER_assume(M > 30);
  __VERIFIER_assume(O > 10);
  int arr[N][M][O];
  arr[15][25][2] = arr[10][15][2];
  while(1)
  {
    __VERIFIER_assert(arr[15][25][2] == arr[10][15][2]); // This should be constant
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/multi_dimensional_array_2
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \barray size in bytes overflows address space size$
// Vereda: verdict none
