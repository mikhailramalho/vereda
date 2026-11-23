extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
extern unsigned char __VERIFIER_nondet_uchar(void);

// No loop: the whole program is the transition system's prefix, so there is no
// state and no step, and check_prefix alone decides it.
int main() {
  unsigned char a = __VERIFIER_nondet_uchar();
  unsigned char b = __VERIFIER_nondet_uchar();
  __VERIFIER_assert((unsigned char)(a + b) == (unsigned char)(b + a));
  return 0;
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_no_loop
// Needs: --error-label ERROR --ts-bmc --ts-no-fallback
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --ts-no-fallback --max-k-step 4
// MODE: FUTURE
// EXPECT: ^Transition system extracted
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
