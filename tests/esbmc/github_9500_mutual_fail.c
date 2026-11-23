#include <assert.h>
int __VERIFIER_nondet_int(void);
void g(_Bool r, int *p);

void f(_Bool r, int *p) {
  int l = __VERIFIER_nondet_int();
  if (r) {
    *p = 20;
    __VERIFIER_assume(l == 30);
  } else {
    g(1, &l);
    assert(l != 20);
  }
}

void g(_Bool r, int *p) { f(r, p); }

int main(void) {
  f(0, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9500_mutual_fail
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
