extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    __VERIFIER_assert(x < 4);
    x = (x + 1) & 3;
  }
}
// Imported from ESBMC regression/esbmc/ts_check_accept
// Needs: --error-label ERROR --ts-check
// RUN: --no-pointer-check --no-bounds-check --error-label ERROR --ts-check
// MODE: FUTURE
// EXPECT: ^TS-CHECK accepted states=1 inputs=0 defs=3 bad=1 prefix_bad=0 extract_s=
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
