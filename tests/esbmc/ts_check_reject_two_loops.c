int __VERIFIER_nondet_int();

int main() {
  int x = 0;
  while (__VERIFIER_nondet_int())
    x++;
  for (;;)
    __VERIFIER_assert(x >= 0); // x stays non-negative
}
// Imported from ESBMC regression/esbmc/ts_check_reject_two_loops
// Needs: --ts-check
// RUN: --no-pointer-check --no-bounds-check --ts-check
// MODE: FUTURE
// EXPECT: ^WARNING: main has more than one loop$
// EXPECT: ^TS-CHECK rejected$
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
