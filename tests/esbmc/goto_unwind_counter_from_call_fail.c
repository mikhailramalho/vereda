// The loop runs 4 times: the call's result moves i past the bound.
int g(int x)
{
  return x == 3 ? 100 : x;
}

int main()
{
  int i, n = 0;
  for (i = 0; i < 10; i++)
  {
    n++;
    i = g(i);
  }
  __VERIFIER_assert(n != 4); // the loop ran 4 times
  return 0;
}
// Imported from ESBMC regression/esbmc/goto_unwind_counter_from_call_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 20
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
