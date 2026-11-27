int main()
{
  int idx;
  __VERIFIER_assume(0 <= idx);
  while (idx < 5)
  {
    ++idx;
  }
  __VERIFIER_assert(idx >= 5); // assert
  return 0;
}
// Imported from ESBMC regression/esbmc/github_interval_guard
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
