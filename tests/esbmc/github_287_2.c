a[][3];
b;
main() {
  int c, d = a[c][2];
  __VERIFIER_assume(d >= 0);
  assert(b);
}
// Imported from ESBMC regression/esbmc/github_287_2
// RUN: --solver z3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
