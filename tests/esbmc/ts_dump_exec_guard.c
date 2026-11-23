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
// Imported from ESBMC regression/esbmc/ts_dump_exec_guard
// Needs: --error-label ERROR --ts-check --ts-dump
// RUN: --no-pointer-check --no-bounds-check --error-label ERROR --ts-check --ts-dump
// MODE: FUTURE
// EXPECT: ^Transition system: 1 state variables, 0 inputs, 3 step definitions,
// EXPECT: ^  \[.*\] error label: goto_symex::guard[?!&#0-9]*$
// Vereda: verdict none
