unsigned int __VERIFIER_nondet_uint(void);

static void poll(void)
{
  unsigned char empty = (unsigned char)__VERIFIER_nondet_uint();
  __VERIFIER_assume(empty == 1);
  if (!empty)
    poll();
}

int main(void)
{
  unsigned int n = __VERIFIER_nondet_uint();
  for (unsigned int i = 0; i < n; i++)
  {
    poll();
    __VERIFIER_assert(i < n); // loop index below bound
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/github_9852
// Needs: --k-induction
// RUN: --k-induction --k-step 4 --max-k-step 9
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: Solution found by the inductive step
// Vereda: verdict none
