extern unsigned char __VERIFIER_nondet_uchar(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    unsigned char in = __VERIFIER_nondet_uchar() & 1;
    __VERIFIER_assert(x != 5);
    if (in)
      x = x + 2;
    else
      x = x != 0 ? x - 1 : 0;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_branches_push_pop_fail
// Needs: --error-label ERROR --ts-bmc --ts-push-pop
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --ts-push-pop --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Transition-system counterexample at step 4
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
