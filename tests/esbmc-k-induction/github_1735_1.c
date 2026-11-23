_Bool a = 1;
_Bool b = 0;

int main() {
  goto c;
d:
  b = a;
c:
  b = a;
  a = 0;
  __VERIFIER_assert(b);
  goto d;
}
// Imported from ESBMC regression/k-induction/github_1735_1
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
