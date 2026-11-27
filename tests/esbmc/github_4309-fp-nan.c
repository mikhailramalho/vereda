#include <assert.h>

float __VERIFIER_nondet_float(void);

// Regression for the IEEE-NaN blocking-clause bug flagged on PR #4310:
// without special handling, plain `==` on floatbv lowers to SMT
// `fp.eq`, which is false on NaN==NaN. The blocking clause then
// degenerates to a tautology, causing duplicate NaN witnesses (or
// non-termination under --max-witnesses=0).
//
// Expected behaviour: at most one NaN witness is reported per claim;
// once NaN is enumerated, the next iteration's blocking clause
// (built via isnan(sym)) excludes the entire NaN class.

int main(void)
{
  float x = __VERIFIER_nondet_float();
  assert(x > 0); // violated by NaN, +/-inf, +/-0, and any negative
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4309-fp-nan
// Needs: --all-witnesses --max-witnesses 4
// RUN: --all-witnesses --max-witnesses 4 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[Counterexamples - 4 witnesses\]
// EXPECT: --max-witnesses cap reached
// Vereda: verdict none
