extern unsigned char __VERIFIER_nondet_uchar(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
void assume_abort_if_not(int c) { if (!c) abort(); }
int main() {
  unsigned char c = __VERIFIER_nondet_uchar();
  assume_abort_if_not(c >= 1 && c <= 3);
  unsigned char y = 0, z = 0;
  for (;;) {
    __VERIFIER_assert(y == (unsigned char)(z * c));
    y = y + c;
    z = z + 1;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_shared_constant
// Needs: --error-label ERROR --ts-bmc
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --max-k-step 8
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
