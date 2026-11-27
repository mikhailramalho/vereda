int __VERIFIER_nondet_int();

int main()
{
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assert(a == a); // a equals itself
  if (a > 0)
    __VERIFIER_assert(a > 0); // positive branch
  else
    __VERIFIER_assert(a <= 0); // non-positive branch
}
// Imported from ESBMC regression/goto-coverage/discussion_5745_all_reached
// Needs: --assertion-coverage-claims
// RUN: --assertion-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: Reached Assertion Instances: 3
// EXPECT: Unreached Asserts: 0
// EXPECT: Assertion Instances Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
