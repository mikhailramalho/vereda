// #10154: a loop the state before it does enter is still proved by the
// inductive step.
int __VERIFIER_nondet_int();

int main()
{
  int i = __VERIFIER_nondet_int();
  int n = 4;
  for (; i < n; ++i)
    __VERIFIER_assert(i < 5);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10154_proved
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
