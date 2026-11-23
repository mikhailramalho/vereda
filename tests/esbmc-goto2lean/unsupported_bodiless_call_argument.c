void g(int);
int main(void) {
  g(*(int *)0);
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_bodiless_call_argument
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported expression dereference$
// Vereda: verdict none
