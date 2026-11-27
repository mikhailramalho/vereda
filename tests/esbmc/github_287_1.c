float a[][6];
int b;
float c;
main() {
  c = a[b][5];
  assert(c);
}
// Imported from ESBMC regression/esbmc/github_287_1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
