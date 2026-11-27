int main()
{
  unsigned a, b;

  __VERIFIER_assume(a < 10);
  __VERIFIER_assume(b < 10);
  __VERIFIER_assert(a + b < 20); // sum stays bounded
  __VERIFIER_assert(a < 10); // a stays bounded
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-promote
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*NOT CHECKED)[\s\S]*\Z
