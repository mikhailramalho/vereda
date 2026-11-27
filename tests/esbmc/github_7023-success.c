int main()
{
  int a, b;

  __VERIFIER_assert(a + b == b + a); // addition commutes
  __VERIFIER_assert(a * 0 == 0); // zero annihilates
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-success
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*NOT CHECKED)[\s\S]*\Z
