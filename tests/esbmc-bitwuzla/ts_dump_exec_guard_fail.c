extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    __VERIFIER_assert(x < 3);
    x = (x + 1) & 3;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_dump_exec_guard_fail
// Needs: --error-label ERROR --ts-bmc
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --error-label ERROR --ts-bmc --max-k-step 8
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
