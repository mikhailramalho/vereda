// #7974: an assume(x == c) lift left x unconstrained under --no-propagation.
int main()
{
  int var1;
  __VERIFIER_assume(var1 == 352005173);
  __VERIFIER_assert(var1 != -7); // symbol == constant

  int x;
  __VERIFIER_assume(3 == x);
  x++;
  __VERIFIER_assert(x == 4); // constant == symbol
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7974
// RUN:
// EXPECT: ^VERIFICATION SUCCESSFUL$
