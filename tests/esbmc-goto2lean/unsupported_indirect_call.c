void h(void) { }
int main(void) {
  ((void (*)(void))0)();
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_indirect_call
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported indirect function call at .*unsupported_indirect_call\.c:3$
// Vereda: verdict none
