unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  return a + b;
}
// Imported from ESBMC regression/smtlib/github_9229_formula
// Needs: --smtlib --output out.smt2 CHECK_FILE out.smt2 contains \(bvuge \S+ \|c:github_9229_formula\.c@\d+@F@main@a\? CHECK_FILE out.smt2 absent \(bvuge \S+ \|c:github_9229_formula\.c@\d+@F@main@b\?
// RUN: --unsigned-overflow-check --smtlib --smt-formula-only --output out.smt2
// MODE: FUTURE
// EXPECT: ^SMT formula written to output file out\.smt2$
// Vereda: verdict none
