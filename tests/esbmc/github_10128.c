/* --bidirectional must not report its own blocking assertion as a bug
   (esbmc/esbmc#10128). */
int main()
{
  unsigned x = 0;
  do
  {
    x++;
    __VERIFIER_assert(x != 6); // six
  } while (x < 5);
}
// Imported from ESBMC regression/esbmc/github_10128
// Needs: --k-induction --bidirectional
// RUN: --k-induction --max-k-step 20 --bidirectional
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
