int main() {
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(y>=0 && y <=10);
  while (y >= 0 && y <= 10) {
    y = (2*y + 1) / 2;
  }     
  return 0;
}
// Imported from ESBMC regression/termination/github_2091
// Needs: --termination
// RUN: --termination
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
