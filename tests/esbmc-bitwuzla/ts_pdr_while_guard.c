// The loop condition reaches PDR as an invariant.
extern unsigned char __VERIFIER_nondet_uchar(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern float __VERIFIER_nondet_float(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  while (x < 100) {
    __VERIFIER_assert(x != 150);
    x = x + __VERIFIER_nondet_bool();
  }
  return 0;
}
// Imported from ESBMC regression/bitwuzla/ts_pdr_while_guard
// Needs: --ts-pdr
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --ts-pdr --max-k-step 20
// MODE: FUTURE
// EXPECT: ^Solution found by PDR
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
