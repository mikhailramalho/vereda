float a;
main() {
  for (;;) {
    memset(&a, 0, sizeof(a)-1);
    reach_error();
    for (;;)
      ;
  }
}
// Imported from ESBMC regression/esbmc/github_1508_non_optimized-2
// Needs: --k-induction
// RUN: --k-induction -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
