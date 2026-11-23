int main()
{
  int x = 0;
  while (1)
  {
  ERROR:
    x = 1;
    break;
  }
  __VERIFIER_assert(x == 1); // the loop sets x before it breaks
  return 0;
}
// Imported from ESBMC regression/esbmc/error_label_loop_head
// Needs: --error-label ERROR
// RUN: --error-label ERROR
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
