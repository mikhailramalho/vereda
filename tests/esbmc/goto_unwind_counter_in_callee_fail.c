// The loop runs 4 times: f moves the global counter past the bound.
int i;

void f()
{
  if (i == 3)
    i = 100;
}

int main()
{
  int n = 0;
  for (i = 0; i < 10; i++)
  {
    n++;
    f();
  }
  __VERIFIER_assert(n != 4); // the loop ran 4 times
  return 0;
}
// Imported from ESBMC regression/esbmc/goto_unwind_counter_in_callee_fail
// Needs: --k-induction --goto-unwind --unlimited-goto-unwind
// RUN: --k-induction --goto-unwind --unlimited-goto-unwind --max-k-step 20
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
