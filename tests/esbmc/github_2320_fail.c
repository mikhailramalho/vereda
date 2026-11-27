// #2320: #pragma comment yields a PragmaCommentDecl that has no runtime form.
#pragma comment(lib, "m")

int main()
{
  int x = 1;
  __VERIFIER_assert(x == 2); // x is two
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2320_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
