int __VERIFIER_nondet_int(void);

int main(void)
{
  int value = __VERIFIER_nondet_int();
  if (value > 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/covered_set_all
// Needs: --branch-coverage-claims --coverage-covered-set covered.json SEED_FILE covered.json {"version":1,"covered":[{"cond":"unrelated","loc":"unrelated"}]} CHECK_JSON covered.json $.version == 1 CHECK_FILE covered.json contains unrelated CHECK_FILE covered.json contains main.c
// RUN: --branch-coverage-claims --no-assertions --coverage-covered-set covered.json
// MODE: FUTURE
// EXPECT: ^Branches : 2$
// EXPECT: ^Reached : 2$
// EXPECT: ^Branch Coverage: 100%$
// Vereda: verdict none
