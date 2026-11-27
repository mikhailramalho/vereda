int main() {
  int arr[100];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_444_2
// Needs: --stack-limit 3199
// RUN: --stack-limit 3199
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
