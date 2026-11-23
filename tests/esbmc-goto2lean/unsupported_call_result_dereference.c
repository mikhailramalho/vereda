unsigned f(void) { return 1; }
int main(void) {
  *(unsigned *)0 = f();
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_call_result_dereference
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported call result stored in dereference at .*unsupported_call_result_dereference\.c:3$
// Vereda: verdict none
