int main()
{
  int a, b;

  __VERIFIER_assert(a + b == b + a); // addition commutes
  /* At most one of these can be violated by any single model, so exactly one
     stays undecided whichever one the solver picks. */
  __VERIFIER_assert(a != 1); // a is not one
  __VERIFIER_assert(a != 2); // a is not two
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-color
// Needs: --result-only --color always
// RUN: --result-only --color always
// MODE: FUTURE
// EXPECT: ^  \x1b\[31mFAILED +\x1b\[0m +\[main\.assertion\.[23]\] +line +[89] +a is not (one|two)$
// EXPECT: ^  \x1b\[33mNOT CHECKED\x1b\[0m +\[main\.assertion\.[23]\] +line +[89] +a is not (one|two)$
// EXPECT: ^\x1b\[31mProperties: 3 checked, \d+ passed, 1 failed, \d+ not checked\x1b\[0m, \x1b\[32m1 passed\x1b\[0m, \x1b\[33m1 not checked\x1b\[0m$
// Vereda: verdict none
