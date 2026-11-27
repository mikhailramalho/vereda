int main()
{
  int a[2];
  a[2] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_664_fail
// Needs: --file-output out.txt CHECK_FILE out.txt contains warning: array index 2 is past the end of the array CHECK_FILE out.txt contains ^VERIFICATION FAILED$
// RUN: --file-output out.txt
// MODE: FUTURE
// EXPECT: ^ESBMC version
// Vereda: verdict none
