// A value-taking option swallows the next --flag as its value (#7525).
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 5)
    goto ERROR;
  return 0;
ERROR:
  return 1;
}
// Imported from ESBMC regression/esbmc/github_7525_option_value
// Needs: --smtlib-solver-prog --all-claims
// RUN: --smtlib-solver-prog --all-claims --unwind 1
// MODE: FUTURE
// EXPECT: ^WARNING: .--smtlib-solver-prog. takes a value, so .--all-claims. was read as that value
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
