#include <assert.h>
int __VERIFIER_nondet_int(void);

int inc(int x) { return x + 1; }

int main(void) {
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k > 0 && k < 10);
  int l = inc(5);
  assert(k < 10);
  return l - 6;
}
// Imported from ESBMC regression/esbmc/github_9500_nonrecursive
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
