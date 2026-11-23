// #9951: an invalid --k-step must exit with an error, not abort().
int main()
{
  return 0;
}
// Imported from ESBMC regression/k-induction/github_9951
// Needs: --k-induction
// RUN: --k-induction --k-step 2 --max-k-step 1
// MODE: FUTURE
// EXPECT: ^ESBMC version
// EXPECT: ^ERROR: --k-step \(2\) must be smaller than --max-k-step \(1\)\.
