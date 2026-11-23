/* Counterpart of ranking_witness: the loop does not terminate. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x = 1;
  return 0;
}
// Imported from ESBMC regression/termination/ranking_witness_fail
// Needs: --termination --k-step 2 --max-inductive-step 3 --no-assertions --witness-output-yaml witness.yml
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --no-assertions --no-pointer-check --no-bounds-check --witness-output-yaml witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Recurrent state shows a non-terminating execution.*$
// Vereda: verdict none
