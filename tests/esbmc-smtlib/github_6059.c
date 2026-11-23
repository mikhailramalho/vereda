int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/smtlib/github_6059
// Needs: --smtlib --output out.smt2 CHECK_FILE out.smt2 contains \(check-sat\) CHECK_FILE out.smt2 contains \(assert CHECK_FILE out.smt2 absent SMT formula dumped successfully
// RUN: --smtlib --smt-formula-only --output out.smt2
// MODE: FUTURE
// EXPECT: ^SMT formula written to output file out\.smt2$
// Vereda: verdict none
