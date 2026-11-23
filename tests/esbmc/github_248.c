void a();
void b() {
  for (;;)
    a();
}
void a() { b(); }
// Imported from ESBMC regression/esbmc/github_248
// Needs: --k-induction
// RUN: --k-induction --function b
// MODE: FUTURE
// EXPECT: ^WARNING: k-induction does not support recursion yet. Disabling inductive step$
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
