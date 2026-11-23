/* A real violation is still found under --bidirectional, and is reported
   against the program's assertion (esbmc/esbmc#10128). */
int main()
{
  unsigned x = 0;
  do
  {
    x++;
    __VERIFIER_assert(x != 5); // five
  } while (x < 5);
}
// Imported from ESBMC regression/esbmc/github_10128_fail
// Needs: --k-induction --bidirectional
// RUN: --k-induction --max-k-step 20 --bidirectional
// MODE: FUTURE
// EXPECT: ^  five$
// EXPECT: ^Bug found \(k = 5\)$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
