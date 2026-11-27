int __VERIFIER_nondet_int(void);
int calls;

void fail(void)
{
  __VERIFIER_assert(0); // callee assertion
  calls++;
}

int main()
{
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  fail();
  if (a > 0 && b > 0)
    return 2;
  if (a < 0 || b < 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/assert_false_callee_rm
// Needs: --condition-coverage-rm
// RUN: --condition-coverage-rm --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  10
// EXPECT: ^Total Conditions:  10
// EXPECT: ^Condition Properties - SATISFIED:  9
// EXPECT: ^Condition Properties - UNSATISFIED:  1
// EXPECT: ^Condition Coverage: 90%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
