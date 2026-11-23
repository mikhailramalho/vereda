extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    __VERIFIER_assert(x != 4);
    x = x + 1;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_bmc_fresh_solver_fail
// Needs: --error-label ERROR --ts-bmc --ts-fresh-solver
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --ts-fresh-solver --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Transition-system counterexample at step 4
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
