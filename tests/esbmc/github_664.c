int main()
{
  int x = 1;
  __VERIFIER_assert(x == 1); // x is 1
  return 0;
}
// Imported from ESBMC regression/esbmc/github_664
// Needs: --verbosity clang:9 --file-output out.txt CHECK_FILE out.txt contains ^#include <\.\.\.> search starts here:$ CHECK_FILE out.txt contains ^VERIFICATION SUCCESSFUL$
// RUN: --verbosity clang:9 --file-output out.txt
// MODE: FUTURE
// EXPECT: ^ESBMC version
// Vereda: verdict none
