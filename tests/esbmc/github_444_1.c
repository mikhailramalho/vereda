int main() {
  int arr[100];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_444_1
// Needs: --stack-limit 3200
// RUN: --stack-limit 3200 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
