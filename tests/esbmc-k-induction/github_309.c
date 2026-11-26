extern void abort(void);
#include <assert.h>
void reach_error() { assert(0); }

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: {reach_error();abort();}
  }
  return;
}
#define a (2)
extern unsigned int __VERIFIER_nondet_uint();
int main() { 
  int i, j=10, n=__VERIFIER_nondet_uint(), sn=0;
  if (n==2147483647) {
    return 0;
  }
  for(i=1; i<=n; i++) {
    if (i<j) 
    sn = sn + a;
    j--;
  }
  __VERIFIER_assert(sn==((long long) n)*a || sn == 0);
}

// Imported from ESBMC regression/k-induction/github_309
// Needs: --state-hashing --k-induction --max-inductive-step 3 --interval-analysis
// RUN: --no-div-by-zero-check --force-malloc-success --state-hashing --no-align-check --k-step 2 --unlimited-k-steps --k-induction --max-inductive-step 3 --no-pointer-check --no-bounds-check --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
