// The falsifying half of ts_pdr_negative_init: x climbs from -3 to 0, so PDR
// must find the violation three steps in and BMC must replay it.
extern void abort(void);
extern _Bool __VERIFIER_nondet_bool(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int main()
{
  signed char x = -3;
  for (;;)
  {
    __VERIFIER_assert(x < 0);
    if (__VERIFIER_nondet_bool() && x < 100)
      x = x + 1;
  }
}
// Imported from ESBMC regression/bitwuzla/ts_pdr_negative_init_fail
// Needs: --ts-pdr
// RUN: --bitwuzla --no-pointer-check --no-bounds-check --ts-pdr --max-k-step 20
// MODE: FUTURE
// EXPECT: ^PDR found a violation at step 3; replaying with BMC$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
