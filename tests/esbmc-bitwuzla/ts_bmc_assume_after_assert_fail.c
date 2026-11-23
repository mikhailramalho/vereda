extern unsigned char __VERIFIER_nondet_uchar(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
void assume_abort_if_not(int c) { if (!c) abort(); }
int main() {
  for (;;) {
    unsigned char in = __VERIFIER_nondet_uchar();
    __VERIFIER_assert(in != 3);
    assume_abort_if_not(in != 3);
  }
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_assume_after_assert_fail
// Needs: --error-label ERROR --ts-bmc
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Transition-system counterexample at step 0
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
