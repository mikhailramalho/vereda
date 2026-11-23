// The loop runs 4 times: the write through p moves i past the bound.
int main()
{
  int i, n = 0;
  int *p = &i;
  for (i = 0; i < 10; i++)
  {
    n++;
    if (i == 3)
      *p = 100;
  }
  __VERIFIER_assert(n != 4); // the loop ran 4 times
  return 0;
}
// Imported from ESBMC regression/esbmc/goto_unwind_counter_via_pointer_fail
// Needs: --k-induction --goto-unwind --unlimited-goto-unwind
// RUN: --k-induction --goto-unwind --unlimited-goto-unwind --max-k-step 20
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
