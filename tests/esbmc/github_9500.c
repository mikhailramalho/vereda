#include <assert.h>
int __VERIFIER_nondet_int(void);

void rec(_Bool r, int *p) {
  int l = __VERIFIER_nondet_int();
  if (r) {
    *p = 20;
    __VERIFIER_assume(l == 30);
  } else {
    rec(1, &l);
    assert(l == 20);
  }
}

int main(void) {
  rec(0, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9500
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
