/* esbmc/esbmc#10913: several Boolector re-solves until the blocking clauses
   exhaust the input tuples. */
int __VERIFIER_nondet_int();
int main()
{
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x <= 3 && y >= 0 && y <= 3);
  __VERIFIER_assert(x + y != 3); // x + y != 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10913_two_inputs
// Needs: --all-witnesses
// RUN: --all-witnesses
// EXPECT: ^\[Counterexamples - 4 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 4 witnesses
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
