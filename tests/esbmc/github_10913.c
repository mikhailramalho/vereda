/* esbmc/esbmc#10913: --all-witnesses pushes a frame for its blocking clauses,
   which Boolector rejects unless it was created incremental. */
int __VERIFIER_nondet_int();
int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x <= 10);
  __VERIFIER_assert(x != 5); // x != 5
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10913
// Needs: --all-witnesses
// RUN: --all-witnesses
// EXPECT: ^\[Counterexamples - 1 witnesses\]$
// EXPECT: x = 5; //
// EXPECT: enumeration stopped: UNSAT after 1 witnesses
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
