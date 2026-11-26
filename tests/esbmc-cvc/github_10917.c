/* esbmc/esbmc#10917: --all-witnesses reads array elements from the model
   inside its solver frame. */
int __VERIFIER_nondet_int();
int main()
{
  int a[3];
  for (int i = 0; i < 3; i++)
    a[i] = __VERIFIER_nondet_int();
  __VERIFIER_assume(
    a[0] >= 0 && a[0] <= 5 && a[1] >= 0 && a[1] <= 5 && a[2] >= 0 && a[2] <= 5);
  __VERIFIER_assert(a[0] + a[1] + a[2] != 5); // sum
  return 0;
}
// Imported from ESBMC regression/cvc/github_10917
// Needs: --all-witnesses --max-witnesses 4
// RUN: --all-witnesses --max-witnesses 4 --solver cvc5
// EXPECT: ^\[Counterexamples - 4 witnesses\]$
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
