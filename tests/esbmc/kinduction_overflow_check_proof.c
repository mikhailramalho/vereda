// The inductive step proves an unbounded loop free of overflow.
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int i = 0;
  while (__VERIFIER_nondet_int())
  {
    if (i < 100)
      i = i + 1;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/kinduction_overflow_check_proof
// Needs: --k-induction
// RUN: --overflow-check --k-induction --max-k-step 6
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Solution found by the inductive step
// Vereda: verdict none
