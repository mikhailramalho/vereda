unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned c = __VERIFIER_nondet_uint(), x = 0;
  if (c) {
  L:
    x = x + 1;
  }
  if (x < c) {
    __VERIFIER_assert(x != 0); // nz
    goto L;
  }
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_loop_leaves_branch
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported loop that leaves its enclosing branch at .*unsupported_loop_leaves_branch\.c:6$
// Vereda: verdict none
