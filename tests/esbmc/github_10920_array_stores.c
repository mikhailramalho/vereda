/* esbmc/esbmc#10920: the stores are sliced, so the inputs appear only where
   the claim reads them. */
int __VERIFIER_nondet_int();
int main()
{
  int a[2];
  a[0] = __VERIFIER_nondet_int();
  a[1] = __VERIFIER_nondet_int();
  __VERIFIER_assume(a[0] >= 0 && a[0] <= 3 && a[1] >= 0 && a[1] <= 3);
  __VERIFIER_assert(a[0] + a[1] != 3); // a[0] + a[1] != 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_array_stores
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 4 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 4 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
