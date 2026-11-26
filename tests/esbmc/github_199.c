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
// Needs: --error-label ERROR --smt-symex-guard
// RUN: --error-label ERROR --smt-symex-guard --z3 --unwind 1 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
