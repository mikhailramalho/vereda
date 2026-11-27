int main()
{
  double d = -0.0;
  __VERIFIER_assume(d == 0.0);
  assert(signbit(d));
}
// Imported from ESBMC regression/esbmc/github_2757
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
