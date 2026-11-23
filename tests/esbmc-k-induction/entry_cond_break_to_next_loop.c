// The break lands on the next loop's head, which that loop's transformation
// has already turned into a havoc block; it still leaves this loop.
int __VERIFIER_nondet_int();
int main()
{
  int k = __VERIFIER_nondet_int(), c = 0, i = 0, j = 0;
  if (k < 0 || k > 100)
    return 0;
  while (i++ < 1000)
  {
    if (!(c < k))
      break;
    c++;
  }
  while (j < 10)
    j++;
  __VERIFIER_assert(c <= k); // c <= k
}
// Imported from ESBMC regression/k-induction/entry_cond_break_to_next_loop
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
