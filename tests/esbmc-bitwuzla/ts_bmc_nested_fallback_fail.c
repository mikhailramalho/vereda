extern unsigned char __VERIFIER_nondet_uchar(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    while (__VERIFIER_nondet_uchar())
      x = x + 1;
    __VERIFIER_assert(x != 3);
  }
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_nested_fallback_fail
// Needs: --error-label ERROR --ts-bmc
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --max-k-step 10
// MODE: FUTURE
// EXPECT: ^WARNING: main has more than one loop$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
