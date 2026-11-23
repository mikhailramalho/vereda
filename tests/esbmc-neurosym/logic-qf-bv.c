int main()
{
  unsigned x = __VERIFIER_nondet_uint() % 10;
  __VERIFIER_assert(x < 10); // x in range
  return 0;
}
// Imported from ESBMC regression/neurosym/logic-qf-bv
// Needs: --neurosym
// RUN: --neurosym --smt-formula-only
// MODE: FUTURE
// EXPECT: \(set-logic QF_BV\)
// EXPECT: \(check-sat\)
// EXPECT: SMT formula written to standard output
// Vereda: verdict none
