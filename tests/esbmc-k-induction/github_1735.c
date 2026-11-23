_Bool a = 1;
_Bool b = 0;

int main() {
  goto c;
d:
  __VERIFIER_assert(b);
c:
  b = a;
  a = 0;
  goto d;
}
// Imported from ESBMC regression/k-induction/github_1735
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
