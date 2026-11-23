// For 1 <= n <= 9 the loop goes back to i = 0 and runs forever; for any other
// n it ends. The inductive step cannot show this: from most states the loop
// exits.
int __VERIFIER_nondet_int(void);

int main(void)
{
  int i = 0, n = __VERIFIER_nondet_int();
  while (i < 10)
  {
    i++;
    if (i == n)
      i = 0;
  }
  return 0;
}
// Imported from ESBMC regression/termination/recurrent_state_cycle
// Needs: --state-hashing --add-symex-value-sets --k-step 2 --no-assertions --termination --max-inductive-step 3 --interval-analysis
// RUN: --no-div-by-zero-check --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --no-vla-size-check --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// MODE: FUTURE
// EXPECT: ^Recurrent state shows a non-terminating execution
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
