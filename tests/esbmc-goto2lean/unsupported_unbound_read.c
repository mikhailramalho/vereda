unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned c = __VERIFIER_nondet_uint();
  if (c) goto L;
  unsigned y = 1;
L:
  y = y + 1;
  __VERIFIER_assert(y != 0); // y
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_unbound_read
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported read of y where it is not bound$
// Vereda: verdict none
