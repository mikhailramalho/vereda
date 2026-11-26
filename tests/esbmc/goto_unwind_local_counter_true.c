// A local counter whose address is never taken is still unrolled, even with
// a call and a write through a pointer in the body: with no loop left, k=1
// proves the claim.
int g(int x)
{
  return x;
}

int main()
{
  int a[3];
  int *p = a;
  for (int i = 0; i < 3; i++)
    p[i] = g(i);
  __VERIFIER_assert(a[2] == 2); // unrolled
  return 0;
}
// Imported from ESBMC regression/esbmc/goto_unwind_local_counter_true
// Needs: --k-induction
// RUN: --k-induction --max-k-step 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
