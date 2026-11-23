// x takes 2^32 iterations to repeat, too many for the recurrent-state check,
// but no state reaches the exit, which the inductive step shows.
unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();
  while (1)
    x++;
  return 0;
}
// Imported from ESBMC regression/termination/inductive_step_diverging
// Needs: --termination --k-step 2 --max-inductive-step 3 --witness-output-yaml -
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Inductive step shows a non-terminating execution.*$
// EXPECT: ^CWE: CWE-835$
// Vereda: verdict none
