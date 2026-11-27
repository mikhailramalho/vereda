#include "assert.h"
int main() {
  int offset, length, nlen = __VERIFIER_nondet_int();
  int i, j;

  for (i=0; i<nlen; i++) {
    for (j=0; j<8; j++) {
      assert(0 <= nlen-1-i);
      assert(nlen-1-i < nlen);
    }
  }
  return 0;
}

// Imported from ESBMC regression/esbmc/github_199
// RUN: --error-label ERROR --solver z3 --unwind 1 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
