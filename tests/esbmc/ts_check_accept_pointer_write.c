// The loop writes y only through p, and p never moves: the havoc covers *p,
// so y is state.
int main() {
  int y = 0;
  int *p = &y;
  unsigned char x = 0;
  for (;;) {
    __VERIFIER_assert(y < 4); // y bounded
    *p = x;
    x = (x + 1) & 3;
  }
}
// Imported from ESBMC regression/esbmc/ts_check_accept_pointer_write
// Needs: --ts-check
// RUN: --no-pointer-check --no-bounds-check --ts-check
// MODE: FUTURE
// EXPECT: ^TS-CHECK accepted states=2 inputs=0 defs=2 bad=1 prefix_bad=0 extract_s=
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
