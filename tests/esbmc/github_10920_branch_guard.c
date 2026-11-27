/* esbmc/esbmc#10920: a[0] reaches the claim only through the branch guard. */
int __VERIFIER_nondet_int();
void f(int x)
{
  __VERIFIER_assert(x != 0); // x != 0
}
int main()
{
  int a[2];
  a[0] = __VERIFIER_nondet_int();
  a[1] = __VERIFIER_nondet_int();
  if (a[0] >= 0 && a[0] < 3)
    f(a[1]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_branch_guard
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 3 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 3 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
