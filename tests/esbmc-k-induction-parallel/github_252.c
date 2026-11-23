a() {
  return 0;
  return 0;
  return 0;
  b();
}
c() {
  for (;;)
    a();
}
// Imported from ESBMC regression/k-induction-parallel/github_252
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel --function c -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
