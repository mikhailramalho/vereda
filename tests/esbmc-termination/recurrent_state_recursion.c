// The loop calls a recursive function, whose writes the check cannot bound, so
// it leaves the loop alone.
int rec(int n)
{
  return n ? rec(n - 1) : 0;
}

int main(void)
{
  int x = 0;
  while (x < 3)
    x += 1 + rec(2);
  return 0;
}
// Imported from ESBMC regression/termination/recurrent_state_recursion
// Needs: --termination --max-inductive-step 3 --no-assertions
// RUN: --termination --k-step 2 --max-k-step 12 --max-inductive-step 3 --no-pointer-check --no-bounds-check --no-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
