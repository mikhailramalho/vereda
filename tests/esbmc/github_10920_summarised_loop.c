/* esbmc/esbmc#10920: the summarised loop makes one array-valued input, which
   has to be blocked element by element. */
int __VERIFIER_nondet_int();
int main()
{
  int a[2];
  for (int i = 0; i < 2; i++)
    a[i] = __VERIFIER_nondet_int();
  __VERIFIER_assume(a[0] >= 0 && a[0] <= 3 && a[1] >= 0 && a[1] <= 3);
  __VERIFIER_assert(a[0] + a[1] != 3); // a[0] + a[1] != 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_summarised_loop
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^\[Counterexamples - 4 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 4 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
