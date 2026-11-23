int __VERIFIER_nondet_int();

int main() {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 5); // x is never five
  return 0;
}
// Imported from ESBMC regression/esbmc/ts_check_accept_no_loop
// Needs: --ts-check
// RUN: --no-pointer-check --no-bounds-check --ts-check
// MODE: FUTURE
// EXPECT: ^TS-CHECK accepted states=0 inputs=0 defs=0 bad=0 prefix_bad=1 extract_s=
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
