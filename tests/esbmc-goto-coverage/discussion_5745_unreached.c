int __VERIFIER_nondet_int();

/* Branches hold two assertions each so goto_convert's guarded-assertion
   folding (which only fires on a single assert(false) branch body) does not
   apply: the asserts must stay behind the branch for the unreached-claims
   listing to observe their reachability. */
int main()
{
  if (3 > 0)
  {
    __VERIFIER_assert(1); // reachable concrete then
  }
  else
  {
    __VERIFIER_assert(0); // dead concrete else A
    __VERIFIER_assert(1); // dead concrete else B
  }

  int a = __VERIFIER_nondet_int();
  if (a % 2)
    ++a;
  __VERIFIER_assert(a % 2 == 0); // a is even

  if (a % 2)
  {
    __VERIFIER_assert(0); // dead symbolic then A
    __VERIFIER_assert(1); // dead symbolic then B
  }
  else
  {
    __VERIFIER_assert(1); // reachable symbolic else
  }
}
// Imported from ESBMC regression/goto-coverage/discussion_5745_unreached
// Needs: --assertion-coverage-claims
// RUN: --assertion-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: Reached Assertion Instances: 3
// EXPECT: Unreached Asserts: 4
// EXPECT: Assertion Instances Coverage: 42.8
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
