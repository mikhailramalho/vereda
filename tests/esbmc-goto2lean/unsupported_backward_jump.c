unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned c = __VERIFIER_nondet_uint(), x = 0;
  goto M;
L:
  x = x + 1;
M:
  if (x < c) goto L;
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_backward_jump
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported backward conditional jump at .*unsupported_backward_jump\.c:8$
// Vereda: verdict none
