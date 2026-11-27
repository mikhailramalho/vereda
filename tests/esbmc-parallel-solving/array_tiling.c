b, c;
main()
{
  long *a = malloc(b);
  a[c] = a[c];
  if (a[c])
  d:
    reach_error();
}
// Imported from ESBMC regression/parallel-solving/array_tiling
// Needs: --k-induction
// RUN: --k-induction --max-k-step 5
// MODE: FUTURE
// EXPECT: ^Bug found \(k = 1\)$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*Unable to prove or falsify the program)[\s\S]*\Z
// Vereda: verdict none
