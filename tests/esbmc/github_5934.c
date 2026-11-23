// Under --smt-formula-only every claim returns P_SMTLIB, so neither assertion
// is ever solved. multi_property_check used to leave final_result at its
// P_UNSATISFIABLE seed, closing the run VERIFICATION SUCCESSFUL and exit 0
// over an analysis that never ran — even though "b" is violable (issue #5934).
int main(void)
{
  int x;
  __VERIFIER_assume(x > 0);
  __VERIFIER_assert(x > 0); // a
  __VERIFIER_assert(x > 1); // b
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5934
// RUN: --multi-property --dump-smt-only --solver z3
// EXPECT: ^Solving claim 'b
// EXPECT: ^\(check-sat\)$
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)[\s\S]*\Z
// Vereda: unmet EXPECT: ^Solving claim 'b
// MODE: FUTURE
// Vereda: verdict none
