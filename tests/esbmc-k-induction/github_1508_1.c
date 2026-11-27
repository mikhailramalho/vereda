float a;
main() {
  for (;;) {
    memset(&a, 0, sizeof(a));
    reach_error();
    for (;;)
      ;
  }
}
// Imported from ESBMC regression/k-induction/github_1508_1
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
