/* The shift amount's bit pattern has its top bit set, so a decimal model
   renders it as a negative numeral. local_eval_bv() must mask every model
   value to the symbol's width before using it, or the guard on the shift
   arms (rhs >= width) never fires for it and mp_arith's operator<< reaches
   power(2, negative). */
struct wrap
{
  unsigned int f;
};

int main()
{
  unsigned int x = __VERIFIER_nondet_uint();
  unsigned int y = __VERIFIER_nondet_uint();
  struct wrap w;

  __VERIFIER_assume(y > 0x80000000u);
  w.f = x << y;
  __VERIFIER_assert(w.f != 0u); // shift by at least the width is non-zero
  return 0;
}
// Imported from ESBMC regression/neurosym/decimal-model-shift
// Needs: --neurosym --neurosym-prog fake-neurosym.sh --neurosym-model-prog z3 -in
// RUN: --neurosym --neurosym-prog fake-neurosym.sh --neurosym-model-prog "z3 -in"
// MODE: FUTURE
// EXPECT: ^\[Counterexample\]$
// EXPECT: ^  w\.f = 0; // 00000000 00000000 00000000 00000000$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
