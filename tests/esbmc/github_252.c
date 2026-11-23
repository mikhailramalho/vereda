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
// Imported from ESBMC regression/esbmc/github_252
// Needs: --k-induction
// RUN: --k-induction --function c -Wno-error=implicit-function-declaration -Wno-error=implicit-int
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
