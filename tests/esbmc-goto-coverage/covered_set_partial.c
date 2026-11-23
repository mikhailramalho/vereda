int __VERIFIER_nondet_int(void);

int main(void)
{
  int value = __VERIFIER_nondet_int();
  __VERIFIER_assume(value > 0);
  if (value > 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/covered_set_partial
// Needs: --branch-coverage-claims --no-assertions --coverage-covered-set covered.json SEED_FILE covered.json {"version":1,"covered":[{"cond":"unrelated","loc":"unrelated"}]} CHECK_JSON covered.json $.version == 1 CHECK_FILE covered.json contains unrelated CHECK_FILE covered.json contains covered_set_partial\.c
// RUN: --branch-coverage-claims --no-assertions --coverage-covered-set covered.json
// MODE: FUTURE
// EXPECT: ^Branches : 2$
// EXPECT: ^Reached : 1$
// EXPECT: ^Branch Coverage: 50%$
// Vereda: verdict none
