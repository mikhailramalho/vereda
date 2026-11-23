// p moves inside the loop, so the havoc covers p itself, and a pointer cannot
// be TS state.
int main() {
  int a[2] = {0, 0};
  int *p = a;
  unsigned char x = 0;
  for (;;) {
    __VERIFIER_assert(a[0] < 4); // a bounded
    *p = x;
    p = (p == a) ? a + 1 : a;
    x = (x + 1) & 3;
  }
}
// Imported from ESBMC regression/esbmc/ts_check_reject_moving_pointer_write
// Needs: --ts-check
// RUN: --no-pointer-check --no-bounds-check --ts-check
// MODE: FUTURE
// EXPECT: ^WARNING: havocked state holds a pointer or an empty aggregate$
// EXPECT: ^TS-CHECK rejected$
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
