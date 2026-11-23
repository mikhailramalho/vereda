// The falsifying half of ts_pdr_branches: x can reach 11.
extern unsigned char __VERIFIER_nondet_uchar(void);
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
int main() {
  unsigned char x = 0;
  for (;;) {
    unsigned char in = __VERIFIER_nondet_uchar() & 1;
    __VERIFIER_assert(x != 11);
    if (in && x < 10)
      x = x + 2;
    else
      x = x != 0 ? x - 1 : 0;
    x = x & 15;
  }
}
// Imported from ESBMC regression/z3/ts_pdr_branches_fail
// Needs: --ts-pdr
// RUN: --z3 --no-pointer-check --no-bounds-check --ts-pdr --max-k-step 20
// MODE: FUTURE
// EXPECT: ^PDR found a violation at step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
