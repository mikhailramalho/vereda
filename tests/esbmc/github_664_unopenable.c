int main()
{
  int x = 1;
  __VERIFIER_assert(x == 1); // x is 1
  return 0;
}
// Imported from ESBMC regression/esbmc/github_664_unopenable
// Needs: --file-output missing-dir/out.txt
// RUN: --file-output missing-dir/out.txt
// MODE: FUTURE
// EXPECT: ^ERROR: Cannot open missing-dir/out.txt$
// Vereda: verdict none
