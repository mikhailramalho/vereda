#include <assert.h>

int main() {
  for (;;) {
    for (int i=0;
         i<2;
         i++)
      ;
    assert(-1 > 0);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1508
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
