/* --bidirectional renumbers the program mid-run; one assertion stays one row
   (esbmc/esbmc#7900). */
int main()
{
  unsigned x = 0;
  while (x < 10)
  {
    x += 2;
    __VERIFIER_assert(x != 11); // inloop
  }
  __VERIFIER_assert(x == 10); // after
  return 0;
}
// Imported from ESBMC regression/esbmc/same_position_assert_bidirectional
// Needs: --k-induction --bidirectional
// RUN: --k-induction --max-k-step 8 --bidirectional --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
