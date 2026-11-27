/* esbmc/esbmc#10920: inputs are numbered in call order, whichever pass finds
   them: y is assigned to a live variable, a[0] and a[1] are read in place. */
int __VERIFIER_nondet_int();
int main()
{
  int a[2];
  a[0] = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  a[1] = __VERIFIER_nondet_int();
  __VERIFIER_assume(y == 10 && a[1] >= 0 && a[1] <= 1 && a[0] >= 5 && a[0] <= 6);
  __VERIFIER_assert(a[0] + a[1] + y != 16); // a[0] + a[1] + y != 16
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_input_order
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^    #1 : \[0\] = [56], \[1\] = 10, \[2\] = [01]$
// EXPECT: ^    #2 : \[0\] = [56], \[1\] = 10, \[2\] = [01]$
// EXPECT: enumeration stopped: UNSAT after 2 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
