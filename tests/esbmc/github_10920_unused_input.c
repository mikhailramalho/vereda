/* esbmc/esbmc#10920: an input the claim does not read is not enumerated. */
int __VERIFIER_nondet_int();
int main()
{
  int unused = __VERIFIER_nondet_int();
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x <= 10);
  __VERIFIER_assert(x != 5); // x != 5
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_unused_input
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 1 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 1 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
