#include <assert.h>

int a = 1;
int b = 0;

int main() {
  goto c;
d:
  __VERIFIER_assume(a == 2);
c:
  b = a;
  a = 0;
  assert(b == 0 || b == 3);
  goto d;
}
// Imported from ESBMC regression/k-induction/github_1735_3
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
