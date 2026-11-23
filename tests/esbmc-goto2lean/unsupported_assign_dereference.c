int main(void) {
  *(int *)0 = 1;
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_assign_dereference
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported assignment to dereference at .*unsupported_assign_dereference\.c:2$
// Vereda: verdict none
