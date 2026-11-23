#include <assert.h>

_Bool a = 1;
_Bool b = 0;

int main() {
  goto c;
d:
  assert(b == 0 || b == 1);
c:
  b = a;
  a = 0;
  goto d;

  return 0;
}
// Imported from ESBMC regression/k-induction/github_1735_4
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
