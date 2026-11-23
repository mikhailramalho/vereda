// PDR works on bit-vector cubes, so float state ends UNKNOWN rather than in a verdict.
extern unsigned char __VERIFIER_nondet_uchar(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern float __VERIFIER_nondet_float(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  float f = 0.0f;
  for (;;) {
    __VERIFIER_assert(f >= 0.0f);
    f = f + 1.0f;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_pdr_float_state_unknown
// Needs: --ts-pdr
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --ts-pdr --max-k-step 20
// MODE: FUTURE
// EXPECT: ^PDR: state component f is not a bit-vector of at most 64 bits$
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
