// The falsifying half of ts_pdr_while_guard: x reaches 50 before the loop exits.
extern unsigned char __VERIFIER_nondet_uchar(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern float __VERIFIER_nondet_float(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  while (x < 100) {
    __VERIFIER_assert(x != 50);
    x = x + __VERIFIER_nondet_bool();
  }
  return 0;
}
// Imported from ESBMC regression/bitwuzla/ts_pdr_while_guard_fail
// Needs: --ts-pdr
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --ts-pdr --max-k-step 20
// MODE: FUTURE
// EXPECT: ^PDR found a violation at step 50; replaying with BMC$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
