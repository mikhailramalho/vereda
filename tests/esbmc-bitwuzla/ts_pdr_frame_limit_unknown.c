// Safe, but needs more frames than --max-k-step allows.
extern unsigned char __VERIFIER_nondet_uchar(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern float __VERIFIER_nondet_float(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    __VERIFIER_assert(x != 12);
    if (__VERIFIER_nondet_bool() && x < 10)
      x = x + 2;
    else
      x = x != 0 ? x - 1 : 0;
    x = x & 15;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_pdr_frame_limit_unknown
// Needs: --ts-pdr
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --ts-pdr --max-k-step 2
// MODE: FUTURE
// EXPECT: ^PDR: frame limit reached$
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
