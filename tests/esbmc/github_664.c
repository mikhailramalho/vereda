int main()
{
  int x = 1;
  __VERIFIER_assert(x == 1); // x is 1
  return 0;
}
// Imported from ESBMC regression/esbmc/github_664
// Needs: --file-output out.txt
// RUN: --file-output out.txt
// MODE: FUTURE
// EXPECT: ^ESBMC version
// Vereda: verdict none
