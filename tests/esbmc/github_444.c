int main() {
  int arr[100];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_444
// Needs: --stack-limit 3000
// RUN: --stack-limit 3000
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
