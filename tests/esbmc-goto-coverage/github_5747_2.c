int __VERIFIER_nondet_int();

int main()
{
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a % 2 == 0);
  if (a % 2)
  {
    ++a;
    __VERIFIER_assert(0); // unreachable odd branch
  }
  __VERIFIER_assert(a % 2 == 0); // a is even here
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_5747_2
// Needs: --assertion-coverage-claims
// RUN: --assertion-coverage-claims
// MODE: FUTURE
// EXPECT: Reached Assertion Instances: 1
// EXPECT: Assertion Instances Coverage: 50%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
